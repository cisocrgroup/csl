#!/usr/bin/perl

use strict;

package Cislex;

use IPC::Open2;

sub new {
    my $self = {};
    ( my $class, %$self ) = @_;

    if( !$self->{alphFile} || !$self->{dicFile} ) {
	print STDERR "Cislex: provide arguments 'alphFile' and 'dicFile' as arguments for the constructor.\n";
	return undef;
    }

    my $binary = "~/implement/csl/trunk/bin/lookupCislex $self->{alphFile} $self->{dicFile}";

    open2( $self->{BINARY_OUT}, $self->{BINARY_IN}, $binary ) or die "Perl::Cislex: $!";

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
