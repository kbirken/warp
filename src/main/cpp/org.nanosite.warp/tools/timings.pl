#!/usr/bin/perl

my $n = 1000;

my $tStart = time();
for my $i (1..$n) {
   # execute warp simulator here
}
my $tEnd = time();

my $tDelta = $tEnd-$tStart;
print "overall time: $tDelta\n";
print "runs: $n\n";
print "time per run: ", $tDelta/$n, "\n";

