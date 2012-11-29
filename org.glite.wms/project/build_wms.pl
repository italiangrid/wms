#!/usr/bin/perl

use strict;
use Getopt::Std;
#use strict 'vars';

my %opts = {};

getopts('ht:d:r:p:DCcM', \%opts);

my $opt = "";
my $F = "0";
my $L = "13";
my @components = ();
my $command = "";
my $me = "";

$me = $0;
my @rel = ();
my @pieces = ();
my $SL = 0;
my $SLVER = "";
my $DEB = 0;
my $slver = "";

if (-e "/etc/redhat-release") {
#  print "Detected RehHat-like Linux release...\n";
  @rel = `/bin/cat /etc/redhat-release`;

#  print "rel=$rel[0]\n";

  @pieces = split(/\s+/, $rel[0]);
  
  if($pieces[0] =~ m/^scientific.*/i)
  {
#    print "Detected Scientific Linux release...\n";
  } else {
    die "Invalid Linux release $pieces[0]. Stop.\n\n";
  }
  $slver = $pieces[$#pieces-1];
#  print "$slver\n\n";
  if($slver !~ m/^6.*/ && $slver !~ m/^5.*/) {
    die "Found Scientific Linux Version $SLVER but only 6.x and 5.x are supported. Stop.\n\n"
  }
  $SLVER = (split(/\./, $slver))[0];
  $SL=1;
  print "\nDetected Scientific Linux $SLVER ...\n\n";
}

if(-e "/etc/debian_version") {
  @rel = `/bin/cat /etc/debian_version`;
  @pieces = split(/\./, $rel[0]);
  if($pieces[0] ne "6") {
    die "At the moment a Debian version different than 6.x.x is not supported. Stop.\n\n"
  }
  $DEB = 1;
  print "\nDetected Debian Linux release 6...\n";
}

if($DEB==0 && $SL==0) {
  die "Detected a platform that is not Scientific Linux or Debian. Stop";
}
@components = (
"org.glite.wms.configuration",
"org.glite.wms.common",
"org.glite.wms.purger",
"org.glite.wms.core",
"org.glite.wms.jobsubmission",
"org.glite.wms.interface",
"org.glite.wms.ice",
"org.glite.wms.nagios",
"org.glite.wms",
"org.glite.wms.brokerinfo-access",
"org.glite.wms.wmproxy-api-cpp",
"org.glite.wms.wmproxy-api-python",
"org.glite.wms-ui.api-python",
"org.glite.wms-ui.commands"
);

my $i = 0;

if( defined $opts{h}) {
  print "Usage: buid_wms.pl [OPTIONS] <start_component_name_to_build> <last_component_name_to_build>\n";
  print "\nOptions:\n";
  print "  -h\tprints this help and exits\n";
  print "  -t\t<tag>: specify the tag to build (default=master)\n";
  print "  -d\t<build-dir>: specify the build directory (default=$ENV{HOME})\n";
  print "  -r\t<emi release number>: specifiy the EMI release number (1, 2, 3, default=2)\n";
  print "  -p\t<platform name>: specify the build platform (sl5,sl6,deb6, default=autodetect)\n";
  print "  -D\tspecify if download and install external dependencies (default=NO)\n";
  print "  -C\tspecify if checkout from GIT must be done before start build (default=NO)\n";
  print "  -c\tspecify if a previous cleanup must be done before start build (default=NO)\n";
  print "  -M\tspecify if execute the mock build (default=NO)\n\n";
  print "  <start_component_name_to_build> <last_component_name_to_build> must be numbers\n";
  print "  specified in according with the following mapping:\n";
  for ($i = 0; $i < @components; $i++) {
      print "  [$i]\t->\t$components[$i]\n";
  }  
  exit;
}

my $tag = "master";
if(defined $opts{t}) {
    $tag = $opts{t};
}

my $dir = $ENV{HOME};
if(defined $opts{d}) {
  $dir = $opts{d};
}

my $emirel = "2";
if(defined $opts{r}) {
  $emirel = $opts{r};
}

my $platform = "";#"sl6";
if($SL) {
  if($SLVER eq "6") {
    $platform = "sl6";
  } else {
    $platform = "sl5"
  }
}

if($DEB) {
  $platform = "deb6";
}

if(defined $opts{p}) {
  $platform = $opts{p};
}

my $want_ext_deps=0;
if(defined $opts{D}) {
  $want_ext_deps = 1;
}

my $want_checkout=0;
if(defined $opts{C}) {
  $want_checkout = 1;
}

my $want_cleanup=0;
if(defined $opts{c}) {
  $want_cleanup = 1;
}

my $want_mock=0;
if(defined $opts{M}) {
  $want_mock = 1;
} 






if(not defined $ARGV[0] and not defined $ARGV[1]) {
  print "\n*** Must specify first and last components to build in number format:\n" ;
  for ($i = 0; $i < @components; $i++) {
    print "[$i]\t->\t$components[$i]\n";
  }
  die "\n*** Stop!";
}

$F = $ARGV[0];
$L = $ARGV[1];

if($F>$L) {
  die "\n*** First package number to build must less than last. Stop!\n\n";
}
my $showlast = @components;
$showlast--;
if($F <0 or $L>@components-1) {
  die "\n*** First or last package numbers are out of range (0-$showlast). Stop!\n\n";
}

print "\n...Launching core build script with options:\n\n";
#print "build_wms.sh $tag $dir $emirel $platform $want_ext_deps $want_checkout $want_cleanup $want_mock $F $L\n\n";

$me =~ s/\.pl/\.sh/;

$command = "$me $tag $dir $emirel $platform $want_ext_deps $want_checkout $want_cleanup $want_mock $F $L\n\n";

print "$command\n\n";
exec($command)
