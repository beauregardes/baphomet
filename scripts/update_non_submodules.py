################################################################################
# This script will update the non-submodule thirdparty files
#
# Certain libraries like ImGui are not published with any real structure, it's
# basically loose files inside a repository and it's up to the user to put the
# files in their project correctly
#
# This will look in the non_submodule_listing.json file, which contains a
# listing of repositories and files to pull from those repositories. It
# downloads them to a temporary folder and compares them with the corresponding
# local file (if it exists). It will not rewrite files which have not changed
# since the last time the files were pulled--this helps not bloat the commit
#
# For now only GitHub and GitLab files are supported
#

import hashlib
import json
import os
import pathlib
import shutil
import tempfile
import urllib.request

HERE = pathlib.Path(__file__).parent.resolve()
THIRDPARTY_FOLDER = HERE.parent / "thirdparty"


def get_file_hash(filename):
    """Calculate an MD5 hash for a given file"""
    with open(filename, 'rb') as f:
        file_hash = hashlib.md5()
        # Read 8192 byte chunks to save memory
        while chunk := f.read(8192):
            file_hash.update(chunk)
    return file_hash.hexdigest()


def get_raw_url(site, repository, branch, filename):
    """Construct the raw file URL for a file in the repo"""
    if site == "github":
        return f"https://raw.githubusercontent.com/{repository}/{branch}/{filename}"
    elif site == "gitlab":
        return f"https://gitlab.com/{repository}/-/raw/{branch}/{filename}"
    else:
        raise ValueError(f"Unrecognized site '{site}'; must be either 'gitlab' or 'github'")


def main():
    with open(f"{HERE}/non_submodule_listing.json", 'r') as f:
        listing = json.loads(f.read())

    with tempfile.TemporaryDirectory() as tmp_dir:
        count = 0
        for name, details in listing.items():
            if count > 0:
                print()  # Blank space between repo info blocks
            count += 1

            print(f"Updating {name}\n"
                  f"* Site: {details['site']}\n"
                  f"* Repository: {details['repository']}\n"
                  f"* Branch: {details['branch']}\n"
                  "----------")

            for subdir, files in details['files'].items():
                for f in files:
                    print(f"* {f}... ", end='')

                    # Get the temporary file path and create any intermediate directories
                    tmp_path = f"{tmp_dir}/{f}"
                    tmp_dir_base = os.path.dirname(os.path.abspath(tmp_path))
                    os.makedirs(tmp_dir_base, exist_ok=True)

                    # Get the local file path and create any intermediate directories
                    local_path = f"{THIRDPARTY_FOLDER}/{name}/{subdir}/{f}"
                    local_dir_base = os.path.dirname(os.path.abspath(local_path))
                    os.makedirs(local_dir_base, exist_ok=True)

                    # Attempt to download the file from the repo
                    try:
                        url = get_raw_url(details['site'], details['repository'], details['branch'], f)
                        urllib.request.urlretrieve(url, tmp_path)
                    except Exception as ex:
                        print(f"Failed to retrieve; ({type(ex).__name__}): {str(ex)}; skipping")
                        continue

                    # Calculate the MD5 hash for comparison
                    repo_hash = get_file_hash(tmp_path)
                    local_hash = ''
                    if os.path.isfile(local_path):
                        local_hash = get_file_hash(local_path)
                    if repo_hash == local_hash:
                        print('Skipping')
                    else:
                        shutil.move(tmp_path, local_path)
                        print('Done')
            print('Finished!')


if __name__ == '__main__':
    main()
