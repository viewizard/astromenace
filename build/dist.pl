#!/usr/bin/perl
# $Id: $
$dirname = 'openastromenace';
$arcname = 'oamenace-src';
$distdir = 'distready/';
$version ||= $ARGV[0];
print("no version"),
exit() if !$version or $version =~ /^r/i;
$dirname = "$dirname-$version";
print "$version\n";
`rm -Rf ./$dirname`;
`svn export https://openastromenace.svn.sourceforge.net/svnroot/openastromenace $dirname`;
mkdir($distdir, 777) unless -e $distdir;
$arcname = "$distdir$arcname-$version";
$arcext = ".tar.bz2";
print "create $arcname$arcext \n";
`rm -f $arcname$arcext`;
`tar cjf $arcname$arcext $dirname`;
