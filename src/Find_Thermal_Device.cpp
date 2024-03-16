#include <iostream>
#include <string>
#include <libudev.h>
#include <algorithm>

bool comparePaths(const std::string& path1, const std::string& path2) {
    size_t pos1 = path1.find_last_of("/");
    size_t pos2 = path2.find_last_of("/");
    if (pos1 == std::string::npos || pos2 == std::string::npos) {
        return false;
    }
    std::string path1Suffix = path1.substr(pos1 + 1);
    std::string path2Suffix = path2.substr(pos2 + 1);
    size_t numPos1 = path1Suffix.find("video");
    size_t numPos2 = path2Suffix.find("video");
    if (numPos1 == std::string::npos || numPos2 == std::string::npos) {
        return false;
    }
    int num1 = std::stoi(path1Suffix.substr(numPos1 + 5));
    int num2 = std::stoi(path2Suffix.substr(numPos2 + 5));
    return num1 < num2;
}

int main() {
    const std::string vendorId = "04b4";
    const std::string productId = "f8f8";
    const std::string deviceDescription = "Cypress Semiconductor Corp. GuideCamera";

    struct udev *udev = udev_new();
    if (!udev) {
        std::cerr << "无法初始化 udev" << std::endl;
        return 1;
    }

    struct udev_enumerate *enumerate = udev_enumerate_new(udev);
    if (!enumerate) {
        std::cerr << "无法创建 udev 枚举器" << std::endl;
        udev_unref(udev);
        return 1;
    }

    // 添加匹配规则：使用设备的厂商 ID、产品 ID 和描述信息
    udev_enumerate_add_match_property(enumerate, "ID_VENDOR_ID", vendorId.c_str());
    udev_enumerate_add_match_property(enumerate, "ID_MODEL_ID", productId.c_str());
    udev_enumerate_add_match_property(enumerate, "ID_MODEL", deviceDescription.c_str());
    udev_enumerate_scan_devices(enumerate);

    struct udev_list_entry *devices = udev_enumerate_get_list_entry(enumerate);
    struct udev_list_entry *entry;

    std::string maxVideoPath = "";

    udev_list_entry_foreach(entry, devices) {
        const char *path = udev_list_entry_get_name(entry);
        struct udev_device *device = udev_device_new_from_syspath(udev, path);

        const char *deviceName = udev_device_get_sysname(device);
        if (deviceName) {
            const char *devicePath = udev_device_get_devnode(device);
            if (devicePath && std::string(deviceName).find("video") != std::string::npos) {
                std::cout << "设备名称: " << deviceName << std::endl;
                std::cout << "设备路径: " << devicePath << std::endl;
                if (maxVideoPath.empty() || comparePaths(devicePath, maxVideoPath)) {
                    maxVideoPath = devicePath;
                }
            }
        }

        udev_device_unref(device);
    }

    if (!maxVideoPath.empty()) {
        std::cout << "最大路径视频设备: " << maxVideoPath << std::endl;
    } else {
        std::cout << "未找到 Cypress Semiconductor Corp. GuideCamera 的视频设备" << std::endl;
    }

    udev_enumerate_unref(enumerate);
    udev_unref(udev);

    return 0;
}
