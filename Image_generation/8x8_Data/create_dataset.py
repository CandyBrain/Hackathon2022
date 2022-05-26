import numpy as np
from pathlib import Path
from torch.utils import data
from torchvision import transforms
import torch


class Dataset(data.Dataset):
    def __init__(
        self,
        folder,
        image_size,
        name_lookup_table,
        exts=["csv"],
        use_name_as_label=False,
    ):
        super().__init__()
        self.folder = folder  # ./8x8_Data/name/images
        self.name = folder.split("\\")[-2]
        if use_name_as_label:
            self.name_lookup_table = {name: name for name in name_lookup_table}
        else:
            self.name_lookup_table = name_lookup_table
        self.image_size = image_size
        self.paths = [p for ext in exts for p in Path(f"{folder}").glob(f"**/*.{ext}")]
        self.pos_labels = torch.as_tensor(
            np.array([int(str(p).split("\\")[-1][2]) for p in self.paths]),
            dtype=torch.long,
        )
        self.transform = transforms.Compose(
            [
                # transforms.Resize(image_size),
                # transforms.RandomHorizontalFlip(),
                # transforms.CenterCrop(image_size),
                transforms.ToTensor(),
            ]
        )

    def __len__(self):
        return len(self.paths)

    def __getitem__(self, index):
        path = self.paths[index]
        pos_label = self.pos_labels[index]
        img = np.genfromtxt(path, dtype=np.int16, delimiter=",")  # 0~1023
        return (
            torch.div(self.transform(img), 1023),
            self.name_lookup_table[self.name],
            pos_label,
        )  # 0~1 로 normalize


def make_total_dataset(
    image_size,
    name_lookup_table,
    folder=None,
    exts=["csv"],
    use_name_as_label=False,
    is_train=True,
):
    """
    folder: 8x8_Data의 상위폴더 지정
    """
    PATH = str(Path.cwd())
    Bool_dict = {True: "images", False: "test_images"}
    if folder is None:
        folder = PATH
    names = name_lookup_table.keys()
    return data.ConcatDataset(
        [
            Dataset(
                f"{folder}\8x8_Data\{name}\{Bool_dict[is_train]}",
                image_size=image_size,
                name_lookup_table=name_lookup_table,
                use_name_as_label=use_name_as_label,
                exts=exts,
            )
            for name in names
        ]
    )


if __name__ == "__main__":
    name_dict = {"Cham": 1, "Lee1": 2, "You": 3}
    total_dataset = make_total_dataset(
        8, name_dict, folder=None, exts=["csv"], use_name_as_label=False
    )
    # print(total_dataset[1000])
