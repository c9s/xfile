<?php
if ( ! extension_loaded('fileutil') && ! function_exists('xfile_pathsplit') ) {

define('SIZE_KB', 1024);
define('SIZE_MB', 1048576);
define('SIZE_GB', 1073741824);

function xfile_findbin($bin, $pathstr = null)
{
    $paths = explode(':', $pathstr ?: getenv('PATH') );
    foreach( $paths as $path ) {
        if( file_exists($path . DIRECTORY_SEPARATOR . $bin) ) {
            return $path . DIRECTORY_SEPARATOR . $bin;
        }
    }
    return false;
}

function xfile_pathsplit($path)
{
    return explode(DIRECTORY_SEPARATOR, $path);
}

function xfile_scanpath($path)
{
    if ( ! file_exists($path) )
        return false;
    if ( is_file($path) )
        return false;

    $list = scandir($path);
    foreach( $list as &$item ) {
        if ( "." === $item || ".." === $item ) 
            continue;
        $item = $path . DIRECTORY_SEPARATOR . $item;
    }
    return $list;
}

function xfile_scanpath_dir($path)
{
    if ( ! file_exists($path) )
        return false;
    if ( is_file($path) )
        return false;


    $list = scandir($path);
    $newlist = array();
    foreach( $list as $item ) {
        if ( "." === $item || ".." === $item ) 
            continue;

        if ( is_dir($path . DIRECTORY_SEPARATOR . $item) ) {
            $newlist[] = $path . DIRECTORY_SEPARATOR . $item;
        }
    }
    return $newlist;
}

function xfile_ctime_compare($filename1, $filename2)
{
    $t1 = filectime($filename1);
    $t2 = filectime($filename2);
    if ( $t1 > $t2 ) {
        return 1;
    } elseif ( $t1 == $t2 ) {
        return 0;
    } elseif ( $t1 < $t2 ) {
        return -1;
    }
    return false;
}

function xfile_mtime_compare($filename1, $filename2)
{
    $t1 = filemtime($filename1);
    $t2 = filemtime($filename2);
    if ( $t1 > $t2 ) {
        return 1;
    } elseif ( $t1 == $t2 ) {
        return 0;
    } elseif ( $t1 < $t2 ) {
        return -1;
    }
    return false;
}

function xfile_lastmtime($filelist)
{
    $lastmtime = 0;
    foreach($filelist as $file) {
        $mtime = filemtime($file);
        if ( $mtime > $lastmtime ) 
            $lastmtime = $mtime;
    }
    return $lastmtime;
}

function xfile_lastctime($filelist)
{
    $lastctime = 0;
    foreach($filelist as $file) {
        $ctime = filectime($file);
        if ( $ctime > $lastctime ) 
            $lastctime = $ctime;
    }
    return $lastctime;
}

function xfile_pathjoin($a = null)
{
    if( ! is_array($a) ) {
        $a = func_get_args();
    }
    $len = count($a);
    $len1 = $len - 1;
    for ( $i = 0;  $i < $len ; $i++ ) {
        if ( $i < $len1 ) {
            $a[$i] = rtrim($a[$i],DIRECTORY_SEPARATOR);
        }
        if ( $i > 0 ) {
            $a[$i] = ltrim($a[$i],DIRECTORY_SEPARATOR);
        }
    }
    return join(DIRECTORY_SEPARATOR , $a);
}


function xfile_mkdir_if_not_exists($path, $mask = null , $recurse = null)
{
    if ( ! file_exists( $path ) ) {
        return mkdir($path, $mask, $recurse);
    }
    return false;
}

function xfile_unlink_if_exists($path)
{
    if ( file_exists($path) ) {
        return unlink($path);
    }
    return false;
}

function xfile_rmdir_if_exists($path)
{
    if ( file_exists($path) ) {
        return rmdir($path);
    }
    return false;
}


function xfile_rmtree($path)
{
    foreach (
        $iterator = new RecursiveIteratorIterator(
        new RecursiveDirectoryIterator($path, RecursiveDirectoryIterator::SKIP_DOTS),
        RecursiveIteratorIterator::CHILD_FIRST) as $fileinfo )
    {
        if( $fileinfo->isDir() ) {
            rmdir( $fileinfo->getPathname() );
        } elseif ($fileinfo->isFile()) {
            unlink( $fileinfo->getPathname() );
        }
    }
    if ( is_dir($path) ) {
        rmdir($path);
    } elseif ( is_file($path) ) {
        unlink($path);
    }
    return true;
}


function xfile_paths_remove_basepath($paths,$basepath)
{
    return array_map(function($item) use ($basepath) {
        return substr($item,strlen($basepath));
    }, $paths );
}


function xfile_paths_append(& $paths,$path2, $modify = false)
{
    if ( $modify ) {
        foreach( $paths as & $path ) {
            $path = xfile_pathjoin($path ,$path2);
        }
    } else {
        $newpaths = array();
        foreach( $paths as $path ) {
            $newpaths[] = xfile_pathjoin($path ,$path2);
        }
        return $newpaths;
    }
}

function xfile_paths_prepend(& $paths,$path2, $modify = false)
{
    if ( $modify ) {
        foreach( $paths as & $path ) {
            $path = xfile_pathjoin($path2 ,$path);
        }
    } else {
        $newpaths = array();
        foreach( $paths as $path ) {
            $newpaths[] = xfile_pathjoin($path2 ,$path);
        }
        return $newpaths;
    }
}

/**
 * @param array $paths filter out directories
 */
function xfile_paths_filter_file($paths)
{
    $newpaths = array();
    foreach( $paths as $path ) {
        if ( is_file($path) ) {
            $newpaths[] = $path;
        }
    }
    return $newpaths;
}


/**
 * @param array $paths filter out directories
 */
function xfile_paths_filter_dir($paths)
{
    $newpaths = array();
    foreach( $paths as $path ) {
        if ( is_dir($path) ) {
            $newpaths[] = $path;
        }
    }
    return $newpaths;
}


function xfile_replace_extension($filename, $newext)
{
    $parts = explode('.',$filename);
    if (count($parts) > 1) {
        array_pop($parts);
    }
    $parts[] = $newext;
    return join('.',$parts);
}


function xfile_get_extension($filename)
{
    return pathinfo($filename, PATHINFO_EXTENSION);
}



function xfile_prettysize($bytes)
{
    if ( $bytes < 1024 ) {
        return $bytes . ' B';
    }

    if ( $bytes < SIZE_MB) {
        return sprintf("%d KB", $bytes / SIZE_KB);
    }

    if ( $bytes < SIZE_GB ) {
        return sprintf("%.1f MB", $bytes / SIZE_MB);
    }

    if ( $bytes < SIZE_GB * 1024 ) {
        return sprintf("%.1f GB", $bytes / SIZE_GB );
    }

    return sprintf("%.1f GB", $bytes / SIZE_GB);
}


function xfile_filename_append_suffix( $filename , $suffix )
{
    $pos = strrpos( $filename , '.' );
    if ($pos !== false) {
        return substr( $filename , 0 , $pos )
            . $suffix
            . substr( $filename , $pos );
    }
    return $filename . $suffix;
}

function xfile_get_contents_from_files($files)
{
    $content = '';
    foreach( $files as $file ) {
        if ( file_exists($file) ) {
            if ( $fileContent = file_get_contents($file) ) {
                $content .= $fileContent;
            }
        }
    }
    return $content;
}


function xfile_get_contents_array_from_files($files)
{
    $contents = array();
    foreach( $files as $file ) {
        if ( file_exists($file) ) {
            if ( $fileContent = file_get_contents($file) ) {
                $block = array(
                    'path'    => $file,
                    'content' => $fileContent,
                );
                $contents[] = $block;
            }
        }
    }
    return $contents;
}

function xfile_copy_if_newer($source,$dest) 
{
    if ( file_exists($dest) && ( filemtime($source) < filemtime($dest) ) ) {
        return false;
    }
    copy($source, $dest);
}

function xfile_copy_if_not_exists($source,$dest)
{
    if ( ! file_exists($dest) ) {
        copy($source,$dest);
    }
}


}
