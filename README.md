xfile
===============

[![Build Status](https://travis-ci.org/c9s/xfile.svg?branch=master)](https://travis-ci.org/c9s/xfile)

Fast File Utility Extension (with pure PHP implementation support)

Installation
------------

Install the PHP extension:

```sh
git clone https://github.com/c9s/xfile.git
cd xfile
phpize && ./configure --enable-xfile
make && make install
phpbrew ext enable xfile # if you use phpbrew
```

Synopsis
--------

```php
$filelist = xfile_scanpath("/etc");

$dirlist = xfile_scanpath_dir("/etc");

$path = xfile_pathjoin("etc","folder","file1"); 
// which returns "etc/folder/file1", as the same as "etc" . DIRECTORY_SEPARATOR . "folder" . DIRECTORY_SEPARATOR . "file1"

$subpaths  = xfile_pathsplit("etc/folder/file1");

// replace current extension with another extension.
$newFilename = xfile_replace_extension("manifest.yml","json"); // Returns manifest.json

// get extension from the filename.
$extension = xfile_get_extension("manifest.yml");   // Returns "yml"

// copy file if the mtime of source is newer than the mtime of destination.
xfile_copy_if_newer("source_file","target_file");


// copy file if destination does not exist.
xfile_copy_if_not_exists("source_file", "target_file");


// prepend path to an array that contains paths.
$filelist = array(
    "file1",
    "file2",
    "path2/file3",
    "path3/file4",
    "path4/file5",
);
xfile_paths_prepend($filelist, "/root");
/* $filelist = array(
    "/root/file1",
    "/root/file2",
    "/root/path2/file3",
    "/root/path3/file4",
    "/root/path4/file5",
);
*/


// clean up whole directory
if ( false === xfile_rmtree("/path/to/delete") ) {

}

$newfilename = xfile_filename_append_suffix("Picture.png", "_suffix");  // Returns "Picture_suffix.png"
$newfilename = xfile_filename_append_suffix("Picture", "_suffix");  // Returns "Picture_suffix"

$content = xfile_files_get_contents(array("file1","file2","file3"));

$contents = xfile_files_get_contents_array(array("config.m4","php_xfile.c"));
foreach ($contents as $content) {
    echo $content['path'];
    echo $content['content'];
}


$dirs = xfile_paths_filter_dir($allfiles);

$files = xfile_paths_filter_files($allfiles);
```


Usage
-----
#### `path[] xfile_scanpath($dir)`

The behavior is different from the built-in function `scandir($dir)`,
`xfile_scanpath` concat the items with the base path, and it filters out "." and
".." paths.


```php
$list = xfile_scanpath('/etc');

/*
array( 
    '/etc/af.plist',
    '/etc/afpovertcp.cfg',
    '/etc/asl.conf',
);
*/
```


#### `path[] xfile_scanpath_dir($dir)`

As the same as `xfile_scanpath($dir)`, but `xfile_scanpath_dir` only returns directory entries.


#### `long xfile_lastmtime($filelist)`

This returns the last modification time from a filelist array.


#### `long xfile_lastctime($filelist)`

This returns the last creation time from a filelist array.


#### `string xfile_pathjoin($path1, $path2, $path3)`

#### `array xfile_pathsplit($path)`

#### `xfile_unlink_if_exists($path)`

#### `xfile_rmdir_if_exists($path)`

#### `xfile_mkdir_if_not_exists($path)`

#### `xfile_rmtree($path)`

Recursively delete files and directories through RecursiveDirectoryIterator.

#### `xfile_paths_prepend($paths, $prepend)`

#### `xfile_paths_append($paths, $append)`

#### `xfile_replace_extension($path, $newExtension)`

#### `xfile_get_extension($path)`

#### `xfile_filename_append_suffix($path, $suffix)`


## Hacking

- Fork this project.
- Clone the git repository.

    phpize
    ./configure
    make
    make test


## Author

Yo-An Lin <yoanlin93@gmail.com>

## License

MIT License (See LICENSE file)

