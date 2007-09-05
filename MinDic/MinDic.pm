#!/usr/bin/perl

use strict;

package MinDic;

use IPC::Open2;

sub new {
    my $self = {};
    ( my $class, %$self ) = @_;

    if( !$self->{dicFile} ) {
	print STDERR "MinDic: provide argument 'dicFile' as arguments for the constructor.\n";
	return undef;
    }

    my $binary = "/mounts/Users/student/uli/implement/csl/branches/unicode/bin/lookupMD $self->{dicFile}";

    open2( $self->{BINARY_OUT}, $self->{BINARY_IN}, $binary ) or die "Perl::MinDic: $!";

###############
    binmode($self->{BINARY_OUT}, "utf8:");
    binmode($self->{BINARY_IN}, "utf8:");
##############


    bless( $self, $class );
    return $self;
}

sub close {
    close(BINARY_IN);
    close(BINARY_OUT);
}

sub lookup {
    my($self, $key) = @_;

    print { $self->{BINARY_IN} } ( "$key\n" );

    my $output = readline( $self->{BINARY_OUT} );
    chomp $output;
    return( $output );
}

1;
