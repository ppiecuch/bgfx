#!/bin/bash

#
# This script running following binaries:
# ---------------------------------------
#  - imagemagick convert for creating an assets
#  - unzip
#  - grep
#

function rlink() 
{
  (
  cd $(dirname $1)         # or  cd ${1%/*}
  echo $PWD/$(basename $1) # or  echo $PWD/${1##*/}
  )
}

function generate_asset()
{
    # arguments
    tp=$1;shift
    nm=$1;shift
    txt=$1;shift
    w=$1
    if [ -z "$2" ]; then
	h=$1
    else
	h=$2
    fi

    size="${w}x${h}"

    # stars
    bright=10

    a1=15
    a2=`expr $a1 + 60`
    a3=`expr $a1 + 120`

    b1=`expr $a1 + 180`
    b2=`expr $a2 + 180`
    b3=`expr $a3 + 180`

    # build correct text command:
    if [ ".$tp" == ".icon" ]; then
	txtcmd="-gravity center label:'$txt'"
    elif [ ".$tp" == ".splashscreen" ]; then
	txtcmd="-weight bold -strokewidth 3 -stroke white -gravity north ( -resize 69.8% -fill lightgrey caption:'$txt' ) ( -resize 70% -shade 45x2 -morphology Distance:-1 Euclidean:10,1000 -level 0,10000 caption:'$txt' ) -layers merge"
    else
	txtcmd="label:'$txt'"
    fi

    # escape some chars:
    txtcmd=${txtcmd//'/\\\\\\'}
    txtcmd=${txtcmd//(/\\(}
    txtcmd=${txtcmd//)/\\)}

    convert="convert"
    cv="/opt/local/bin/convert /opt/local/bin/convert /usr/local/bin/convert /usr/bin/convert"
    for c in $cv; do
	if [ -e "$c" ]; then
	    convert=$c
	fi
    done

    eval $convert -size \'$size\' \
	\\\( -seed 4321 plasma:white-blue -background none \\\( $txtcmd \\\) -composite \\\) \
	\\\( xc: -fx \'rand\(\)\' -virtual-pixel tile \
        -colorspace Gray -sigmoidal-contrast 120x0% -negate -blur 0x.3 \
        \\\( -clone 0 -motion-blur 0x10+$a1 -motion-blur 0x10+$b1 \\\) \
        \\\( -clone 0 -motion-blur 0x10+$a2 -motion-blur 0x10+$b2 \\\) \
        \\\( -clone 0 -motion-blur 0x10+$a3 -motion-blur 0x10+$b3 \\\) \
        -compose screen -background none -flatten -normalize \
        -evaluate multiply $bright \\\) \
	-compose plus -composite \
	-bordercolor black -shave 2x2 -border 2x2 -raise 2 \
	$WORKDIR/$nm.png
}

# Allowed arguments:
# ------------------
# DEVICEIP - bb device ip
# DEVICEPW - bb device password
# STOREPW  - store password for package signing
# DEBUGTK  - debug token
# AUTHOR   - author/company name
# AUTHORID - author id (or read from DEBUGTK)
function qnx_bar() 
{
    fn=`basename $APPFILE | sed 's/\(.*\)\..*/\1/'`
    if [ -z "$AUTHORID" ] && [ -z "$DEBUGTK" ]; then
	echo "QNX: Both AUTHORID and DEBUGTK cannot be empty. Terminating.";
	exit 4;
    fi

    if [ -n "$DEBUGTK" ]; then
	expired=`blackberry-debugtokenrequest -verify "$DEBUGTK"|grep EXPIRED`
	if [ -n "$expired" ]; then
	    echo "** Warning: Token is expired - needs to be recreated."
	fi
	authoridTK=`unzip -qq -o "$DEBUGTK" META-INF/MANIFEST.MF && cat META-INF/MANIFEST.MF | grep Author-Id: && rm -rf META-INF`
	authoridTK="${authoridTK#*: }"
	authoridTK="${authoridTK%%[[:space:]]}"
	authorTK=`unzip -qq -o "$DEBUGTK" META-INF/MANIFEST.MF && cat META-INF/MANIFEST.MF | grep Author: && rm -rf META-INF`
	authorTK="${authorTK#*: }"
	authorTK="${authorTK%%[[:space:]]}"
	if [ -z "$AUTHORID" ]; then
	    AUTHORID=$authoridTK
	fi
	if [ -z "$AUTHOR" ]; then
	    AUTHOR=$authorTK
	fi
    fi

    if [ -z "$AUTHORID" ] && [ -z "$authoridTK" ]; then
	echo "QNX: AUTHORID is needed. Terminating.";
	exit 4;
    fi

    if [ -z "$AUTHOR" ] && [ -z "$authorTK" ]; then
	echo "QNX: Warning - No AUTHOR information given.";
    fi

    # qnx is not allowing numbers in id
    # remove it
    old_=$APPID
    APPID=${APPID//[0-9]/}
    APPID=${APPID//._/.}
    APPID=${APPID//../.null.}
    if [ ".$old_" != ".$APPID" ]; then
	echo "** Warning: new APPID is '$APPID'."
    fi

    echo "QNX: creating manifest ..."
    cat <<EOF > $WORKDIR/manifest.xml
<?xml version="1.0" encoding="utf-8" standalone="no"?>
 <qnx xmlns="http://www.qnx.com/schemas/application/1.0">
    <id>$APPID</id>
    <name>$APP</name>
    <versionNumber>$VER</versionNumber>
    <versionLabel>$VER</versionLabel>
    <buildId>$BUILD</buildId>
    <copyright>$COPYRIGHT</copyright>
    <description>$DESCR</description>
   
    <initialWindow>
        <systemChrome>none</systemChrome>
        <transparent>false</transparent>
        <autoOrients>true</autoOrients>
        <aspectRatio>landscape</aspectRatio>
    </initialWindow>
 
    <env var="QML_IMPORT_PATH" value="app/native/imports"/>
    <env var="QT_PLUGIN_PATH" value="app/native/plugins"/>
    <env var="LD_LIBRARY_PATH" value="app/native/lib"/>
 
    <arg>-platform</arg>
    <arg>qnx</arg>
    <publisher>$AUTHOR</publisher>
    <author>$AUTHOR</author>
    <authorId>$AUTHORID</authorId>
    <action system="true">run_native</action>
    <category>$APPCAT</category>
    <asset entry="true" path="$APPFILE" type="Qnx/Elf">$fn</asset>
    <asset path="splashscreen1.png">splashscreen1.png</asset>
    <asset path="splashscreen2.png">splashscreen2.png</asset>
    <asset path="icon1.png">icon1.png</asset>
    <asset path="icon2.png">icon2.png</asset>
    <asset path="icon3.png">icon3.png</asset>
    <asset path="lib">lib</asset>
    <asset path="plugins">plugins</asset>
    <asset path="imports">imports</asset>
    <icon>
     <image>icon1.png</image>
     <image>icon2.png</image>
     <image>icon3.png</image>
    </icon>
    <splashscreens>
      <image>splashscreen1.png</image>
      <image>splashscreen2.png</image>
    </splashscreens>
 </qnx>
EOF

    mkdir -p $WORKDIR/lib
    mkdir -p $WORKDIR/plugins
    mkdir -p $WORKDIR/imports

    echo "QNX: creating assets ..."
    generate_asset "icon" "icon1" "Bgfx demo $SAMPLENR" 86
    generate_asset "icon" "icon2" "Bgfx demo $SAMPLENR" 90
    generate_asset "icon" "icon3" "Bgfx demo $SAMPLENR" 114
    generate_asset "splashscreen" "splashscreen1" "Bgfx demo application $SAMPLENR - $APP" 1280 768
    generate_asset "splashscreen" "splashscreen2" "Bgfx demo application $SAMPLENR - $APP" 1024 600

    # create 'package' subdirectory for destination
    pkgdir=`dirname "$APPFILE"`
    pkgdir="`dirname "$pkgdir"`/package/"

    mkdir -p "$pkgdir"

    if [ -z "$DIST" ]; then
	package="$pkgdir$PACKAGENAME.bar"
    else
	package="$pkgdir${PACKAGENAME}_${VER}.bar"
    fi
    echo "QNX: running blackberry-nativepackager ..."
    if [ -z "$DIST" ]; then
	if [ -z "$DEBUGTK" ]; then
	    blackberry-nativepackager -devMode $package $WORKDIR/manifest.xml || {
		echo "** Warning: command failed with manifest:"
		cat $WORKDIR/manifest.xml
	    }
	else
	    blackberry-nativepackager -devMode -debugToken $DEBUGTK $package $WORKDIR/manifest.xml || {
		echo "** Warning: command  failed with manifest:"
		cat $WORKDIR/manifest.xml
	    }
	fi
    else
	blackberry-nativepackager -sign -cskpass $STOREPW -storepass $STOREPW -package $package $WORKDIR/manifest.xml || {
		echo "** Warning: command failed with manifest:"
		cat $WORKDIR/manifest.xml
	    }
    fi

    if [ -n "$DEPOLY" ]; then
	echo "QNX: running blackberry-deploy ..."
	blackberry-deploy -installApp -device $DEVICEIP -password$DEVICEPW $package
    fi
    echo "QNX: package $package finished"
}


# --- MAIN

WORKDIR='.work'
mkdir -p "$WORKDIR"
if [ ! -d "$WORKDIR" ]; then
    echo "$0: cannot create working directory at $WORKDIR. Terminating."
    exit 1
fi
OUTDIR='.'
mkdir -p "$OUTDIR"
if [ ! -d "$OUTDIR" ]; then
    echo "$0: cannot create output directory at $OUTDIR. Terminating."
    exit 1
fi

#platform
platform=$1
if [ -z "$platform" ]; then
    echo "$0: Missing platform: $0 <qnx|ios|android> args"
    exit 2
fi

shift

if [ "$#" == 0 ]; then
    echo "** Running test ..."
    SAMPLENR="12"
    APP="App"
    APPFILE=$(rlink $0)
    APPID="app.test.id"
    AUTHOR="KomSoft Oprogramowanie"
    AUTHORID="gYAAgOFP83zwpwdTrWwjCuLaPac"
    PACKAGENAME="package-test"
    TEST="1"
fi

# Allowed arguments:
# ------------------
# SAMPLENR   - sample number
# APP        - application name
# APPFILE    - application binary file
# APPID      - application id
# APPCAT     - application category
# DESCR      - application description
# VER        - version information
# BUILD      - build id
# AUTHOR     - author name
# AUTHORID   - qnx author ID
# COPYRIGHT  - copyright information
# DEBUGTK    - qnx debug token
# PACKAGENAME- output package name
# DIST       - package for distribution
# DEPLOY     - deploy after packaging
echo "** Processing arguments ..."
for i in "$@"
do
    k="${i%%=*}"
    case $k in
	SAMPLENR)
	    ;;
	APP)
	    ;;
	APPFILE)
	    ;;
	APPID)
	    ;;
	APPCAT)
	    ;;
	DESCR)
	    ;;
	VER)
	    ;;
	BUILD)
	    ;;
	PACKAGENAME)
	    ;;
	DIST)
	    ;;
	DEPLOY)
	    ;;
	DEBUGTK)
	    ;;
	AUTHOR)
	    ;;
	AUTHORID)
	    ;;
	COPYRIGHT)
	    ;;
	*)
	    echo "$0: Unknown command line argument $k. Terminating."
	    exit 3
    esac
    v="${i#*=}"
    if [ "$i" == "$k" ]; then
	v="1"
    fi
    echo "$k = $v"
    eval $k=$v
done

if [ -z "$APPFILE" ]; then
    echo "$0: APPFILE is required for building a package. Terminating."
    exit 3
fi

# we need a full path
APPFILE=$(rlink "$APPFILE")
if [ -n "$DEBUGTK" ]; then
    DEBUGTK=$(rlink "$DEBUGTK")
fi

# default values
if [ -z "$APPCAT" ]; then
    APPCAT="core.games"
fi

if [ -z "$DESCR" ]; then
    DESCR="Application package: $APP"
fi

if [ -z "$VER" ]; then
    # default value
    VER="0.9"
fi

if [ -z "$BUILD" ]; then
    # create short build id from date
    # qnx doesn't accept long build id
    mj=`date +%y%m`
    mi=`date +%H%M|sed s/^0*//`
    BUILD=$(($mj+$mi))
fi

if [ -n "$APPID" ]; then
    # lets try to cleanup and id
    # replace ' ' and '-' with '_'
    APPID=${APPID// /_}
    APPID=${APPID//-/_}
fi

# --- RUN
echo "** Building package for $APPFILE ..."
case $platform in
    qnx)
	qnx_bar
	;;
    ios)
	;;
    android)
	;;
esac

if [ -z "$TEST" ]; then
    echo "** Cleaning up working directory ..."
    rm -rf "$WORKDIR"
fi
echo "** Done."
