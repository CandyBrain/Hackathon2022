from denoising_diffusion_pytorch import Unet, GaussianDiffusion, Trainer
from pathlib import Path

PATH = str(Path.cwd())


def run_exp(name):
    model = Unet(dim=16, dim_mults=(1, 2)).cuda()

    diffusion = GaussianDiffusion(
        model,
        image_size=8,
        timesteps=125,
        loss_type="l1",  # number of steps  # L1 or L2
    ).cuda()

    trainer = Trainer(
        diffusion,
        f"{PATH}\8x8_Data\{name}\images\\",
        train_batch_size=4,
        train_lr=1e-4,
        train_num_steps=30000,  # total training steps
        gradient_accumulate_every=2,  # gradient accumulation steps
        ema_decay=0.995,  # exponential moving average decay
        amp=True,  # turn on mixed precision
        results_folder=f"{PATH}\8x8_Data\{name}\\results",
    )
    trainer.train()


if __name__ == "__main__":
    run_exp("Lee1")
