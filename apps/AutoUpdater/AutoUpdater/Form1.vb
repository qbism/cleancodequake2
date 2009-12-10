Public Class Form1

    Dim Checker As System.Net.WebClient

    Structure VersionStruct
        Public Str As String
        Public Major As String
        Public Minor As String
        Public Build As String
    End Structure
    Dim Latest As VersionStruct, Current As VersionStruct

    Public Function ReadVersion(ByVal str As String) As VersionStruct
        Dim Vers() As String = str.Split(" ")
        Dim Ver As VersionStruct
        Ver.Str = Vers(0)
        Ver.Major = Vers(1)
        Ver.Minor = Vers(2)
        Ver.Build = Vers(3)
        Return Ver
    End Function

    Public Sub DoneDownloadVersion(ByVal sender As Object, ByVal e As Net.DownloadStringCompletedEventArgs)
        Latest = ReadVersion(e.Result)
    End Sub

    Public Sub GetCurrentVersion()
        Dim Reader As IO.StreamReader = New IO.StreamReader("version.ver")
        Dim str As String = Reader.ReadToEnd()
        Current = ReadVersion(str)
        Reader.Close()
    End Sub

    Private Sub Form1_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        Checker = New System.Net.WebClient
        AddHandler Checker.DownloadStringCompleted, AddressOf DoneDownloadVersion

        Checker.DownloadStringAsync(New System.Uri("http://cleancodequake2.googlecode.com/svn/trunk/version.ver"))

        GetCurrentVersion()
    End Sub
End Class
