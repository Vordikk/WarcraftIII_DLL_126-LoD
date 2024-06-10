package OpenSSL::safe::installdata;

use strict;
use warnings;
use Exporter;
our @ISA = qw(Exporter);
our @EXPORT = qw($PREFIX
                  $BINDIR $BINDIR_REL
                  $LIBDIR $LIBDIR_REL
                  $INCLUDEDIR $INCLUDEDIR_REL
                  $APPLINKDIR $APPLINKDIR_REL
                  $ENGINESDIR $ENGINESDIR_REL
                  $MODULESDIR $MODULESDIR_REL
                  $PKGCONFIGDIR $PKGCONFIGDIR_REL
                  $CMAKECONFIGDIR $CMAKECONFIGDIR_REL
                  $VERSION @LDLIBS);

our $PREFIX             = 'C:\Projects\WAR3\WarcraftIII_DLL_126-xxx\openssl';
our $BINDIR             = 'C:\Projects\WAR3\WarcraftIII_DLL_126-xxx\openssl\apps';
our $BINDIR_REL         = 'apps';
our $LIBDIR             = 'C:\Projects\WAR3\WarcraftIII_DLL_126-xxx\openssl';
our $LIBDIR_REL         = '.';
our $INCLUDEDIR         = 'C:\Projects\WAR3\WarcraftIII_DLL_126-xxx\openssl\include';
our $INCLUDEDIR_REL     = 'include';
our $APPLINKDIR         = 'C:\Projects\WAR3\WarcraftIII_DLL_126-xxx\openssl\ms';
our $APPLINKDIR_REL     = 'ms';
our $ENGINESDIR         = 'C:\Projects\WAR3\WarcraftIII_DLL_126-xxx\openssl\engines';
our $ENGINESDIR_REL     = 'engines';
our $MODULESDIR         = 'C:\Projects\WAR3\WarcraftIII_DLL_126-xxx\openssl\providers';
our $MODULESDIR_REL     = 'providers';
our $PKGCONFIGDIR       = '';
our $PKGCONFIGDIR_REL   = '';
our $CMAKECONFIGDIR     = '';
our $CMAKECONFIGDIR_REL = '';
our $VERSION            = '3.4.0-dev';
our @LDLIBS             =
    # Unix and Windows use space separation, VMS uses comma separation
    split(/ +| *, */, 'ws2_32.lib gdi32.lib advapi32.lib crypt32.lib user32.lib ');

1;
