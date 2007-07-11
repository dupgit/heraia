; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

[Setup]
AppName=H�raia
AppVerName=H�raia 0.0.1
AppPublisher=Olivier Delhomme
AppPublisherURL=http://heraia.tuxfamily.org/
AppSupportURL=http://heraia.tuxfamily.org/
AppUpdatesURL=http://heraia.tuxfamily.org/
DefaultDirName={pf}\Heraia
DefaultGroupName=H�raia
AllowNoIcons=yes
LicenseFile=C:\msys\1.0\home\0659562\heraia\COPYING
OutputDir=C:\Documents and Settings\DELHOMME\Mes documents\builds\Heraia
OutputBaseFilename=heraia_win32_setup_0.0.1
Compression=lzma
SolidCompression=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "french"; MessagesFile: "compiler:Languages\French.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "C:\msys\1.0\home\0659562\heraia\src\heraia.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\msys\1.0\home\0659562\heraia\src\heraia.glade"; DestDir: "{userappdata}\heraia"; Flags: ignoreversion
Source: "C:\mingw\bin\libintl3.dll"; DestDir: "{sys}"; Flags: ignoreversion
Source: "C:\mingw\bin\libiconv2.dll"; DestDir: "{sys}"; Flags: ignoreversion
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{group}\H�raia"; Filename: "{app}\heraia.exe"; WorkingDir: "{app}"
Name: "{group}\{cm:UninstallProgram,H�raia}"; Filename: "{uninstallexe}"
Name: "{commondesktop}\H�raia"; Filename: "{app}\heraia.exe"; Tasks: desktopicon; WorkingDir: "{app}"

