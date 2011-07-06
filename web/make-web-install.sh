#!/bin/sh

path="$1"
force=false
while getopts 'f' opt
do
  case $opt in
    f) force=true
       path="$2" ;;
    ?) printf "Usage: $0: [-f] path\n"
       exit 1 ;;
  esac
done

if [ -z "$1" ];
then
  printf "Usage: $0 [-f] path\n"
  exit 1
fi
if ! $force && [ -d "$path" ];
then
  printf "Path $path already exists. Use -f flag to overwrite.\n"
  exit 1
fi

if $force;
then
  rm -rf $path/*
fi
cp -r . $path

rm -rf $path/data
rm -rf $path/orm
rm -rf `find $path -type d -name .svn`
rm -f $path/.gitignore
rm -f $path/make-web-install.sh

mkdir $path/data
chmod 777 $path/data
chmod 777 $path/lib
chmod 750 $path/private

printf "Finished copying to $path\n"
printf "Please read lib/yahoo-boss-keys-template.json to set up Yahoo Boss credentials\n"
