# ResNet18을 위해 최대한 간단히 수정한 BasicBlock 클래스 정의
import torch
import torch.nn as nn
import torch.nn.functional as F
from torch.utils import data
import torch.backends.cudnn as cudnn
import torch.optim as optim
from torchsummary import summary as torchsumm
from pathlib import Path

from tqdm import tqdm
import create_dataset


class BasicBlock(nn.Module):
    def __init__(self, in_planes, planes, stride=1):
        super(BasicBlock, self).__init__()

        # 3x3 필터를 사용 (너비와 높이를 줄일 때는 stride 값 조절)
        self.conv1 = nn.Conv2d(
            in_planes, planes, kernel_size=3, stride=stride, padding=1, bias=False
        )
        self.bn1 = nn.BatchNorm2d(planes)  # 배치 정규화(batch normalization)

        # 3x3 필터를 사용 (패딩을 1만큼 주기 때문에 너비와 높이가 동일)
        self.conv2 = nn.Conv2d(
            planes, planes, kernel_size=3, stride=1, padding=1, bias=False
        )
        self.bn2 = nn.BatchNorm2d(planes)  # 배치 정규화(batch normalization)

        self.shortcut = nn.Sequential()  # identity인 경우
        if stride != 1:  # stride가 1이 아니라면, Identity mapping이 아닌 경우
            self.shortcut = nn.Sequential(
                nn.Conv2d(in_planes, planes, kernel_size=1, stride=stride, bias=False),
                nn.BatchNorm2d(planes),
            )

    def forward(self, x):
        out = F.relu(self.bn1(self.conv1(x)))
        out = self.bn2(self.conv2(out))
        out += self.shortcut(x)  # (핵심) skip connection
        out = F.relu(out)
        return out


# ResNet 클래스 정의
class ResNet(nn.Module):
    def __init__(self, block, num_blocks, *, in_channels, num_classes):
        super(ResNet, self).__init__()
        self.in_planes = 64

        # 64개의 3x3 필터(filter)를 사용
        self.conv1 = nn.Conv2d(
            in_channels, 64, kernel_size=3, stride=1, padding=1, bias=False
        )
        self.bn1 = nn.BatchNorm2d(64)
        self.layer1 = self._make_layer(block, 64, num_blocks[0], stride=1)
        self.layer2 = self._make_layer(block, 128, num_blocks[1], stride=2)
        self.layer3 = self._make_layer(block, 256, num_blocks[2], stride=2)
        # self.layer4 = self._make_layer(block, 512, num_blocks[3], stride=2)
        self.linear = nn.Linear(256, num_classes)

    def _make_layer(self, block, planes, num_blocks, stride):
        strides = [stride] + [1] * (num_blocks - 1)
        layers = []
        for stride in strides:
            layers.append(block(self.in_planes, planes, stride))
            self.in_planes = planes  # 다음 레이어를 위해 채널 수 변경
        return nn.Sequential(*layers)

    def forward(self, x):
        out = F.relu(self.bn1(self.conv1(x)))
        out = self.layer1(out)
        out = self.layer2(out)
        out = self.layer3(out)
        # out = self.layer4(out)
        out = F.avg_pool2d(out, 2)
        out = out.view(out.size(0), -1)
        out = self.linear(out)
        return out


def ResNet18(*, num_classes):
    model = ResNet(BasicBlock, [1, 1, 1, 1], in_channel=1, num_classes=num_classes)
    print(torchsumm(model), (1, 8, 8), 100)
    return model


class Trainer:
    def __init__(
        self,
        lookup_table,
        num_classes,
        in_channels=1,
        train_batch_size=128,
        train_lr=0.001,
        loss_fn=nn.CrossEntropyLoss(),
        img_size=8,
        train_epochs=15,
        results_folder="./backbone_results",
        adjust_lr=True,
    ) -> None:
        super().__init__()
        self.lookup_table = lookup_table
        self.num_classes = num_classes
        self.model = ResNet(
            BasicBlock,
            [1, 1, 1, 1],
            in_channels=in_channels,
            num_classes=self.num_classes,
        )
        self.batch_size = train_batch_size
        self.train_lr = train_lr
        self.loss_fn = loss_fn
        self.train_epochs = train_epochs
        self.img_size = img_size
        self.train_ds = create_dataset.make_total_dataset(
            self.img_size, self.lookup_table, is_train=True
        )
        self.test_ds = create_dataset.make_total_dataset(
            self.img_size, self.lookup_table, is_train=False
        )
        self.train_dl = data.DataLoader(
            self.train_ds, batch_size=self.batch_size, shuffle=True, pin_memory=True
        )
        self.test_dl = data.DataLoader(
            self.test_ds, batch_size=128, shuffle=False, pin_memory=True
        )
        self.results_folder = Path(results_folder)
        self.results_folder.mkdir(exist_ok=True)
        self.adjust_lr = adjust_lr
        self.curr_epoch = 0

    def save(self, milestone):
        data = {
            "epoch": self.curr_epoch,
            "model": self.model.state_dict(),
            "lookup_table": self.lookup_table,
        }
        torch.save(
            data,
            str(
                self.results_folder
                / f"resnet18-{self.num_classes}classes-{milestone}.pt"
            ),
        )

    def load(self, milestone):
        data = torch.load(
            str(
                self.results_folder
                / f"resnet18-{self.num_classes}classes-{milestone}.pt"
            ),
        )
        self.curr_epoch = data["epoch"]
        self.model.load_state_dict(data["model"])
        self.lookup_table = data["lookup_table"]

    def train(self, device, target_label="name"):
        tgt_dict = {"name": 1, "pos": 2}
        tgt_idx = tgt_dict[target_label]
        cudnn.benchmark = True
        self.model = nn.DataParallel(self.model.to(device))
        optimizer = optim.SGD(
            self.model.parameters(), lr=self.train_lr, momentum=0.9, weight_decay=0.0002
        )
        with tqdm(initial=self.curr_epoch, total=self.train_epochs) as pbar:
            while self.curr_epoch < self.train_epochs:
                # adjust learning rate
                if self.curr_epoch == 5 and self.adjust_lr == True:
                    for param_group in optimizer.param_groups:
                        param_group["lr"] = self.train_lr / 10

                # train
                self.model.train()
                train_loss = 0
                correct = 0
                total = 0
                for batch_idx, batch in enumerate(
                    self.train_dl
                ):  # batch = (inputs, name_label, pos_label)
                    inputs = batch[0].to(device)
                    target = batch[tgt_idx].to(device)
                    total += target.size(0)
                    optimizer.zero_grad()

                    benign_outputs = self.model(inputs)
                    loss = self.loss_fn(benign_outputs, target)
                    loss.backward()

                    optimizer.step()
                    train_loss += loss.item()
                    _, pred = benign_outputs.max(1)

                    correct += pred.eq(target).sum().item()

                    if batch_idx % 100 == 0:
                        print("\nCurrent batch:", str(batch_idx))
                        print(
                            "Current benign train accuracy:",
                            str(pred.eq(target).sum().item() / target.size(0)),
                        )
                        print("Current benign train loss:", loss.item())

                print("\nTotal benign train accuarcy:", 100.0 * correct / total)
                print("Total benign train loss:", train_loss)

                # test
                print("\n[ Test epoch: %d ]" % self.curr_epoch)
                self.model.eval()
                loss = 0
                correct = 0
                total = 0
                for batch_idx, batch in enumerate(
                    self.test_dl
                ):  # batch = (inputs, name_label, pos_label)
                    inputs = batch[0].to(device)
                    target = batch[tgt_idx].to(device)
                    total += target.size(0)

                    outputs = self.model(inputs)
                    loss += self.loss_fn(outputs, target).item()

                    _, predicted = outputs.max(1)
                    correct += predicted.eq(target).sum().item()

                print("\nTest accuarcy:", 100.0 * correct / total)
                print("Test average loss:", loss / total)
                self.save(self.curr_epoch)
                print("Model Saved!")

                # raise epoch
                self.curr_epoch += 1
                pbar.update(1)


if __name__ == "__main__":
    trainer = Trainer(
        {"Lee1": 0, "Lee2": 1, "Moon": 2, "Shin": 3, "You": 4},
        train_batch_size=32,
        num_classes=5,
    )
    trainer.train("cuda", target_label="name")
