import csv
import glob
import os
from pathlib import Path
from numpy import genfromtxt, int16, savetxt

PATH = str(Path.cwd())


def extract_csv(folder):
    sub_dirs = [
        x[0]
        for x in os.walk(folder + "/8x8_Data")
        if "images" not in str(x[0])
        if "samples" not in str(x[0])
        if "results" not in str(x[0])
    ][1:]
    for path in sub_dirs:
        Path.mkdir(Path(f"{path}/images"), exist_ok=True)
        Path.mkdir(Path(f"{path}/test_images"), exist_ok=True)
        csv_paths = [p for p in Path(f"{path}").glob(f"**/*.csv")]
        for csv_path in csv_paths:
            pos = str(csv_path)[-7:-4]
            img_concat = genfromtxt(csv_path, skip_header=1, delimiter=",", dtype=int16)
            for idx, img in enumerate(img_concat):
                if pos in [
                    "Po1",
                    "Po2",
                    "Po3",
                    "Po4",
                    "Po5",
                    "Po6",
                    "Po7",
                    "Po8",
                ]:
                    savetxt(
                        f"{path}/images/{pos}{'_'*(3-len(str(idx)))}{idx}.csv",
                        img.reshape(8, 8),
                        delimiter=",",
                    )
                elif pos == "Po9":
                    savetxt(
                        f"{path}/test_images/{pos}{'_'*(3-len(str(idx)))}{idx}.csv",
                        img.reshape(8, 8),
                        delimiter=",",
                    )


if __name__ == "__main__":
    extract_csv(PATH)
