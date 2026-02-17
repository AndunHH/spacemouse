# Pre-Release Checklist

Before creating a release or merging a pull request the following manual steps shall be done:

1. Run [testConfigCompileSize](testConfigCompileSize.py) to check program size.
2. Update [release.h](spacemouse-keys/release.h) to new version
    1. if the pull request makes a major overhaul or breaks compatibility with older version: increase first number
    2. if the pull request adds a feature: increase second number
    3. if the pull request is committing only minor fixes: increase third number
3. Update [README.md](README.md)