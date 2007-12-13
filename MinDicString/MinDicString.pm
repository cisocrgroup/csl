#!/usr/bin/perl

use strict;

package MinDicString;

use IPC::Open2;

sub new {
    my $self = {};
    ( my $class, %$self ) = @_;


    if( !$self->{alphFile} || !$self->{dicFile} ) {
	print STDERR "MinDicString: provide arguments 'alphFile' and 'dicFile' as arguments for the constructor.\n";
	return undef;
    }

    my $lookupBinary = '/mounts/Users/student/uli/ARBEITSGRUPPE/software_svn/dict_tools/lookupMDString';
    $lookupBinary .= '64' if ( $ENV{HOSTTYPE} eq 'x86_64' );

    
#    my $binary = "~/MA/Zerl_web/lex/csl/trunk/bin/lookupMDString $self->{alphFile} $self->{dicFile}";
    my $binary = "$lookupBinary $self->{alphFile} $self->{dicFile}";
    
    open2( $self->{BINARY_OUT}, $self->{BINARY_IN}, $binary ) or die "Perl::MinDicString: $!";
    

    if( defined $self->{encoding} ) {
	if( $self->{encoding} eq 'iso' ) {
	    $self->{encoding} = 'bytes';
	}
    }
    else { # if nothing is specified, use utf8
	$self->{encoding} = 'utf8';
    }

    binmode( $self->{BINARY_OUT}, ':'.$self->{encoding} );
    binmode( $self->{BINARY_IN}, ':'.$self->{encoding} );

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
