%global __provides_exclude_from ^%{_datadir}/%{name}/lib/.*$
%global __requires_exclude ^lib(dconf|flutter-embedder|maliit-glib|gstreamer-1.0|.+_platform_plugin)\\.so.*$

Name: com.alexsherkan.example
Summary: Demonstrates how to use the video_player plugin.
Version: 0.1.0
Release: 1
License: Proprietary
Source0: %{name}-%{version}.tar.zst

BuildRequires: cmake
BuildRequires: ninja
BuildRequires: pkgconfig(flutter-embedder)

%description
%{summary}.

%prep
%autosetup

%build
%cmake -GNinja -DCMAKE_BUILD_TYPE=%{_flutter_build_type}
%ninja_build

%install
%ninja_install

%files
%{_bindir}/%{name}
%{_datadir}/%{name}/*
%{_datadir}/applications/%{name}.desktop
%{_datadir}/icons/hicolor/*/apps/%{name}.png
