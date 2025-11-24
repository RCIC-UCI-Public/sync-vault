Name:           sync-vault
Version:        1.0
Release:        1%{?dist}
Summary:        Simple RPC client server where the client rings a "doorbell" and requests a server to run a server-defined script. A simple, but practical use is to give a cluster client a starting OTP to connect to Hashicorp vault.
License:        GPL
Source0:        %{name}-%{version}.tar.gz
BuildRequires: gcc
BuildRequires: make
BuildRequires: rpcgen
BuildRequires: automake
BuildRequires: autoconf

%description
In clusters where the server node can ssh to any
node (but not vice-versa), the script can be used to place a starting, one-time password so that the client can
connect to a Hashicorp vault server.  An additional utility, wait-for-file, enables the caller to simply
block (using inotify) for a file to appear in the local file system. In normal use: 1) client requests starting
secret, 2) server securely copies the starting secret to the client. 3) the client waits for the secret to be
deposited.  No secret information is passed through the rpc mechanism itself.  The server is coded so that only 
requests from privileged ports ( < 1024 ) result in the secret being deposited

%prep
%setup -q


%build
aclocal
autoheader
automake --add-missing
autoreconf
./configure --prefix=%{_prefix}
make

%install
rm -rf %{buildroot}
make DESTDIR=%{buildroot} install
%files
%{_prefix}/sbin/*
%changelog
* Fri Nov 21 2025 User <user@example.com> - 1.0-1
- Initial package.

