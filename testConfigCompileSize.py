# Build Automation Script for PlatformIO Projects
# 
# This script automates building a PlatformIO project with multiple configuration header files.
# 
# It works by:
# 
# Iterating over all .h files in the testConfig directory.
# 
# Copying each config file to the target project’s config.h path.
# 
# Running a PlatformIO build (pio run) for each configuration.
# 
# Parsing the build output to extract Flash and RAM usage (both bytes and percentages).
# 
# Recording whether the build succeeded or failed.
# 
# Writing the results into a CSV report (build_report.csv) for easy comparison across configurations.
# 
# This makes it easy to test how different configurations affect memory usage and build success.

import os
import shutil
import re
import csv
import subprocess

CONFIG_DIR = 'testConfig'
CONFIG_TARGET = 'spacemouse-keys/config.h'
PIO_CMD = "pio"              # Adjust if needed, e.g. full path to platformio.exe
BUILD_ARGS = ["run"]
REPORT_FILE = 'testConfig/0_build_report.csv'

OK_MARK = "[OK]"
FAIL_MARK = "[FAIL]"
BUILD_MARK = "[BUILD]"

def parse_build_output(output):
    flash_used = None
    flash_total = None
    ram_used = None
    ram_total = None

    for line in output.splitlines():
        if 'RAM:' in line:
            match = re.search(r'used\s+(\d+)\s+bytes.*?(\d+)\s+bytes', line)
            if match:
                ram_used = int(match.group(1))
                ram_total = int(match.group(2))
        elif 'Flash:' in line or 'Program:' in line:
            match = re.search(r'used\s+(\d+)\s+bytes.*?(\d+)\s+bytes', line)
            if match:
                flash_used = int(match.group(1))
                flash_total = int(match.group(2))

    flash_pct = round(flash_used / flash_total * 100, 1) if flash_used and flash_total else None
    ram_pct = round(ram_used / ram_total * 100, 1) if ram_used and ram_total else None

    return flash_used, ram_used, flash_pct, ram_pct

def run_platformio_capture_output(args):
    try:
        result = subprocess.run(
            [PIO_CMD] + args,
            capture_output=True,
            text=True,
            check=True
        )
        return result.stdout, True
    except subprocess.CalledProcessError as e:
        return e.stdout + "\n" + e.stderr, False

def run_automation():
    configs = [f for f in os.listdir(CONFIG_DIR) if f.endswith('.h')]
    results = []
    all_success = True

    for config_file in configs:
        print(f'\n{BUILD_MARK} Building with {config_file}...')

        config_path = os.path.join(CONFIG_DIR, config_file)
        shutil.copyfile(config_path, CONFIG_TARGET)

        output, success = run_platformio_capture_output(BUILD_ARGS)
        flash, ram, flash_pct, ram_pct = parse_build_output(output)

        if success:
            print(f"{OK_MARK} Build succeeded: Flash={flash} bytes ({flash_pct}%), RAM={ram} bytes ({ram_pct}%)")
        else:
            all_success = False
            print(f"{FAIL_MARK} Build failed for {config_file}")
            if flash or ram:
                print(f"   Info: Detected size before failure: Flash={flash} ({flash_pct}%), RAM={ram} ({ram_pct}%)")
            print(output)

        results.append({
            'Config': config_file,
            'Flash (bytes)': flash if flash is not None else 'N/A',
            'Flash (%)': flash_pct if flash_pct is not None else 'N/A',
            'RAM (bytes)': ram if ram is not None else 'N/A',
            'RAM (%)': ram_pct if ram_pct is not None else 'N/A',
            'Build Success': 'Yes' if success else 'No'
        })

    # Add summary row
    summary_line = f"{OK_MARK} All builds successful" if all_success else f"{FAIL_MARK} Some builds failed"
    results.append({
        'Config': summary_line,
        'Flash (bytes)': '',
        'Flash (%)': '',
        'RAM (bytes)': '',
        'RAM (%)': '',
        'Build Success': ''
    })

    with open(REPORT_FILE, 'w', newline='') as csvfile:
        fieldnames = ['Config', 'Flash (bytes)', 'Flash (%)', 'RAM (bytes)', 'RAM (%)', 'Build Success']
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        writer.writeheader()
        writer.writerows(results)

    print(f'\nReport generated: {REPORT_FILE}')
    print(summary_line)


if __name__ == '__main__':
    run_automation()
