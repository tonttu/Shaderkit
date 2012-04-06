#include "local.iss"

#define ShaderkitName "Shaderkit"
#define ShaderkitURL "http://shaderkit.org/"
#define ShaderkitExeName "shaderkit.exe"

[Setup]
AppId={{CBC588FC-CFCA-4BCC-B419-5C34366F0E75}
AppName={#ShaderkitName}
AppVersion={#ShaderkitVersion}
AppPublisherURL={#ShaderkitURL}
AppSupportURL={#ShaderkitURL}
AppUpdatesURL={#ShaderkitURL}
DefaultDirName={pf}\{#ShaderkitName}
DefaultGroupName={#ShaderkitName}
OutputBaseFilename=Shaderkit {#ShaderkitVersion} {#ShaderkitArchUI}
Compression=lzma
SolidCompression=yes
ChangesAssociations=True
ShowLanguageDialog=no
AppMutex=ShaderkitGlobalMutex
VersionInfoCopyright=© 2010-2012 Riku Palomäki
WizardImageFile=installer-bg.bmp
WizardSmallImageFile=installer-small.bmp

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked; OnlyBelowVersion: 0,6.1

[Files]
Source: "{#ShaderkitRoot}\bin\shaderkit.exe"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "{#ShaderkitRoot}\docs\*"; DestDir: "{app}\docs"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#ShaderkitRoot}\examples\*"; DestDir: "{app}\examples"; Flags: ignoreversion recursesubdirs createallsubdirs

Source: "{#QtRoot}\bin\QtCore{#QtSuffix}.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "{#QtRoot}\bin\QtGui{#QtSuffix}.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "{#QtRoot}\bin\QtOpenGL{#QtSuffix}.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
;Source: "{#QtRoot}\bin\QtWebKit{#QtSuffix}.dll"; DestDir: "{app}\bin"; Flags: ignoreversion

Source: "{#QtRoot}\plugins\imageformats\qgif{#QtSuffix}.dll"; DestDir: "{app}\plugins\imageformats"; Flags: ignoreversion
Source: "{#QtRoot}\plugins\imageformats\qico{#QtSuffix}.dll"; DestDir: "{app}\plugins\imageformats"; Flags: ignoreversion
Source: "{#QtRoot}\plugins\imageformats\qjpeg{#QtSuffix}.dll"; DestDir: "{app}\plugins\imageformats"; Flags: ignoreversion
Source: "{#QtRoot}\plugins\imageformats\qmng{#QtSuffix}.dll"; DestDir: "{app}\plugins\imageformats"; Flags: ignoreversion
Source: "{#QtRoot}\plugins\imageformats\qsvg{#QtSuffix}.dll"; DestDir: "{app}\plugins\imageformats"; Flags: ignoreversion
Source: "{#QtRoot}\plugins\imageformats\qtga{#QtSuffix}.dll"; DestDir: "{app}\plugins\imageformats"; Flags: ignoreversion
Source: "{#QtRoot}\plugins\imageformats\qtiff{#QtSuffix}.dll"; DestDir: "{app}\plugins\imageformats"; Flags: ignoreversion
;Source: "{#QtRoot}\plugins\codecs\*codecs{#QtSuffix}.dll"; DestDir: "{app}\plugins\codecs"; Flags: ignoreversion

Source: "{#VCRedistPath}\{#VCRedist}"; DestDir: "{tmp}"

[Icons]
Name: "{group}\{#ShaderkitName}"; Filename: "{app}\bin\{#ShaderkitExeName}"
Name: "{group}\{cm:ProgramOnTheWeb,{#ShaderkitName}}"; Filename: "{#ShaderkitURL}"
Name: "{group}\{cm:UninstallProgram,{#ShaderkitName}}"; Filename: "{uninstallexe}"
Name: "{commondesktop}\{#ShaderkitName}"; Filename: "{app}\bin\{#ShaderkitExeName}"; Tasks: desktopicon
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\{#ShaderkitName}"; Filename: "{app}\bin\{#ShaderkitExeName}"; Tasks: quicklaunchicon

[Registry]
Root: HKCR; Subkey: ".shaderkit"; ValueType: string; ValueName: ""; ValueData: "Shaderkit"; Flags: uninsdeletevalue
Root: HKCR; Subkey: "Shaderkit"; ValueType: string; ValueName: ""; ValueData: "Shaderkit project file"; Flags: uninsdeletekey
Root: HKCR; Subkey: "Shaderkit\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\bin\{#ShaderkitExeName},0"; Flags: uninsdeletekey
Root: HKCR; Subkey: "Shaderkit\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\bin\{#ShaderkitExeName}"" ""%1"""; Flags: uninsdeletekey
Root: HKLM; Subkey: "Software\Shaderkit"; ValueType: string; ValueName: "Root"; ValueData: "{app}"; Flags: uninsdeletekey

[Code]
function NeedCVRedist(): Boolean;
var
  V: Cardinal;
begin
  Result := TRUE;
  if RegQueryDWordValue(HKLM, 'SOFTWARE\Microsoft\VisualStudio\10.0\VC\VCRedist\{#ShaderkitArch}', 'Installed', V) then
  begin
    Result := V <> 1;
  end;
end;

[Run]
Filename: "{tmp}\{#VCRedist}"; Parameters: "/passive /showfinalerror"; Check: NeedCVRedist
Filename: "{app}\bin\{#ShaderkitExeName}"; Description: "{cm:LaunchProgram,{#StringChange(ShaderkitName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent

