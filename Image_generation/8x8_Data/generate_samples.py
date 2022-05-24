from denoising_diffusion_pytorch import Unet, GaussianDiffusion, Trainer
from pathlib import Path
from torchvision import utils
import torch.nn.functional as F
import random
import torch
from numpy import int16, genfromtxt, savetxt

PATH = str(Path.cwd())


def gen_sample_from_model(name, n_samples=64, as_csv=True, unnormalize=True):
    model = Unet(dim=16, dim_mults=(1, 2)).cuda()

    diffusion = GaussianDiffusion(
        model,
        image_size=8,
        timesteps=125,
        loss_type="l1",  # number of steps  # L1 or L2
    ).cuda()

    trainer = Trainer(
        diffusion,
        f"{PATH}\8x8_Data\gen_samples\\",
        train_batch_size=4,
        train_lr=1e-4,
        train_num_steps=30000,  # total training steps
        gradient_accumulate_every=2,  # gradient accumulation steps
        ema_decay=0.995,  # exponential moving average decay
        amp=True,  # turn on mixed precision'
        results_folder=f"{PATH}\8x8_Data\{name}\\results",
    )
    trainer.load(29)
    trainer.ema_model.eval()
    gens = trainer.ema_model.sample(n_samples)
    BOOL_DICT = {True: "raw", False: "normalized"}
    if as_csv:
        gens = gens.detach().cpu().numpy()
        gens = gens.reshape(n_samples, 64)
        if unnormalize:
            gens = (gens * 1023).round().astype(int16)
        savetxt(
            f"{PATH}\8x8_Data\gen_samples\\{name}_gen_samples_{BOOL_DICT[unnormalize]}.csv",
            gens,
            delimiter=",",
        )

    else:
        utils.save_image(
            gens,
            f"{PATH}\8x8_Data\gen_samples\\{name}_gen_samples.png",
        )


def sample_from_real_data(name):
    paths = [p for p in Path(f"{PATH}\8x8_Data\{name}\images").glob(f"**/*.csv")]
    idxs = [random.randint(0, len(paths) - 1) for _ in range(64)]
    paths = [paths[idx] for idx in idxs]
    imgs = [
        F.pad(
            torch.div(torch.Tensor(genfromtxt(p, dtype=int16, delimiter=",")), 1023),
            (1, 1, 1, 1),
        )
        for p in paths
    ]
    temps = []
    for i in range(8):
        temps.append(torch.cat(imgs[i * 8 : (i + 1) * 8], dim=0))
    temps = F.pad(torch.cat(temps, dim=1), (1, 1, 1, 1))
    print(temps.shape)
    utils.save_image(
        temps, f"{PATH}\8x8_Data\\gen_samples\\{name}_real_samples.png", nrow=8
    )


if __name__ == "__main__":
    gen_sample_from_model("You", 100, as_csv=True, unnormalize=False)
    # sample_from_real_data("You")
