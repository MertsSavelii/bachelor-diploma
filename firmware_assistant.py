import os
import subprocess
import sys
import argparse

def configure_project(project_dir, ssid, password, thingsboard_server, access_token):
    sdkconfig_path = os.path.join(project_dir, "sdkconfig")
    sdkconfig_content = f"""
CONFIG_ESP_WIFI_SSID="{ssid}"
CONFIG_ESP_WIFI_PASSWORD="{password}"
CONFIG_THINGSBOARD_SERVER="{thingsboard_server}"
CONFIG_THINGSBOARD_ACCESS_TOKEN="{access_token}"
"""
    with open(sdkconfig_path, 'w') as sdkconfig_file:
        sdkconfig_file.write(sdkconfig_content)

def build_project(project_dir):
    subprocess.run(["idf.py", "set-target", "esp32"], cwd=project_dir, check=True)
    subprocess.run(["idf.py", "build"], cwd=project_dir, check=True)

def flash_project(project_dir, port):
    subprocess.run(["idf.py", "-p", port, "flash"], cwd=project_dir, check=True)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Build and flash ESP32 project.")
    parser.add_argument("project_dir", help="Directory of the ESP32 project")
    parser.add_argument("ssid", help="WiFi SSID")
    parser.add_argument("password", help="WiFi Password")
    parser.add_argument("thingsboard_server", help="ThingsBoard Server URL")
    parser.add_argument("access_token", help="ThingsBoard Access Token")
    parser.add_argument("port", help="Serial port for flashing")

    if len(sys.argv) == 2 and sys.argv[1] == "-h":
        print("Usage: python build_and_flash.py <project_dir> <ssid> <password> <thingsboard_server> <access_token> <port>")
        sys.exit(0)

    args = parser.parse_args()

    project_dir = args.project_dir
    ssid = args.ssid
    password = args.password
    thingsboard_server = args.thingsboard_server
    access_token = args.access_token
    port = args.port

    configure_project(project_dir, ssid, password, thingsboard_server, access_token)
    build_project(project_dir)
    flash_project(project_dir, port)

    print("Project built and flashed successfully.")
