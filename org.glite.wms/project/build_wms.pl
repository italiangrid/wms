#!/usr/bin/perl

use strict;
use Getopt::Std;
#use strict 'vars';

my %opts = {};

getopts('ht:d:r:p:DCcM', \%opts);

my $opt = "";
my $F = "0";
my $L = "14";
my @components = ();
my $command = "";
my $me = "";

$me = $0;

@components = (
	       "glite-wms-configuration",
	       "glite-wms-common",
	       "glite-wms-purger",
	       "glite-wms-jobsubmission",
	       "glite-wms-core",
	       "glite-wms-interface",
	       "glite-wms-ice",
	       "emi-wms-nagios",
	       "emi-wms",
	       "glite-wms-brokerinfo-access",
	       "glite-wms-wmproxy-api-cpp",
	       "glite-wms-wmproxy-api-java",
	       "glite-wms-wmproxy-api-python",
	       "glite-wms-ui-api-python",
	       "glite-wms-ui-commands"
	      );

# print "-o = $opts{o} . \n" if defined $opts{o};
# print "-D = $opts{D} . \n" if defined $opts{D};
# print "-I = $opts{I} . \n" if defined $opts{I};
# print "-f = $opts{f} . \n" if defined $opts{f};

my $i = 0;

if( defined $opts{h}) {
  print "Usage: buid_wms.pl [OPTIONS] <start_component_name_to_build> <last_component_name_to_build>\n";
  print "\nOptions:\n";
  print "  -h\tprints this help and exits\n";
  print "  -t\t<tag>: specify the tag to build (default=master)\n";
  print "  -d\t<build-dir>: specify the build directory (default=$ENV{HOME})\n";
  print "  -r\t<emi release number>: specifiy the EMI release number (1, 2, 3, default=2)\n";
  print "  -p\t<platform name>: specify the build platform (sl5,sl6,deb6, default=sl6)\n";
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

my $platform = "sl6";
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

if($F>=$L) {
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
