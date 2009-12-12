Imports System
Imports System.IO
Imports System.Net

Public Class Form1

    Structure VersionStruct
        Public Str As String
        Public Major As String
        Public Minor As String
        Public Build As String

        Function GetStr()
            Return Str + "." + Major + "." + Minor + "." + Build
        End Function
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

    ' Returns true if version is same
    Public Function CompareVersions(ByVal left As VersionStruct, ByVal right As VersionStruct)
        If (left.Str = right.Str And left.Major = right.Major And left.Minor = right.Minor And left.Build = right.Build) Then Return True
        Return False
    End Function

    Public Sub GetCurrentVersion()
        Dim Reader As StreamReader = New StreamReader(TextBox1.Text + "\baseq2\version.ver")
        Dim str As String = Reader.ReadToEnd()
        Current = ReadVersion(str)
        Reader.Close()
    End Sub

    Sub ChangeTheNameAndProgress(ByVal name As String)
        Form2.Label2.Text = name
        Form2.ProgressBar1.Style = ProgressBarStyle.Continuous
        Form2.ProgressBar1.Value = Form2.ProgressBar1.Value + 1

        If name = "Finished" Or name = "Failed" Or name = "Done, no changes needed" Then Form2.Button1.Text = "Close"
    End Sub

    Sub ForceBar()
        Form2.ProgressBar1.Value = Form2.ProgressBar1.Maximum
    End Sub

    Sub ChangeTheNameOnly(ByVal name As String)
        Form2.Label2.Text = name
    End Sub

    Delegate Sub ChangeTheNameDelegate(ByVal name As String)
    Public ChangeNameProgress As ChangeTheNameDelegate
    Public ChangeName As ChangeTheNameDelegate

    Delegate Sub ForceBarToFinishSub()
    Public ForceBarToFinish As ForceBarToFinishSub

    Public Sub DoneDLLDownload(ByVal sender As Object, ByVal e As DownloadDataCompletedEventArgs)
        Form2.Invoke(Me.ChangeNameProgress, "Done download, saving...")

        File.Delete(TextBox1.Text + "\baseq2\gamex86.dll")
        Dim BWriter As New BinaryWriter(New FileStream(TextBox1.Text + "\baseq2\gamex86.dll", FileMode.Create))
        BWriter.Write(e.Result)
        BWriter.Close()

        Dim Writer As New StreamWriter(New FileStream(TextBox1.Text + "\baseq2\version.ver", FileMode.Create))
        Writer.Write(Latest.Str + " " + Latest.Major + " " + Latest.Minor + " " + Latest.Build)
        Writer.Close()

        Form2.Invoke(Me.ChangeNameProgress, "Finished")
        Form2.Invoke(Me.ForceBarToFinish)
    End Sub

    Public Sub DoneVersionDownload(ByVal sender As Object, ByVal e As DownloadDataCompletedEventArgs)
        If e.Cancelled = False AndAlso e.Error Is Nothing Then
            Latest = ReadVersion(New System.Text.ASCIIEncoding().GetString(e.Result))

            Form2.Invoke(Me.ChangeNameProgress, "Done download, comparing versions...")

            If CompareVersions(Latest, Current) Then
                MsgBox("Your version is up to date.")
                Form2.Invoke(Me.ChangeNameProgress, "Done, no changes needed")
                Form2.Invoke(Me.ForceBarToFinish)
            Else
                If (MsgBox("An update for your version is available:" + vbNewLine + "Your version: " + Current.GetStr() + vbNewLine + "Update version: " + Latest.GetStr() + vbNewLine + vbNewLine + "Update?", MsgBoxStyle.YesNo) = MsgBoxResult.Yes) Then
                    Form2.Invoke(Me.ChangeNameProgress, "Downloading DLL...")
                    DownloadFile("http://cleancodequake2.googlecode.com/svn/trunk/quake2/baseq2/gamex86.dll", AddressOf DoneDLLDownload)
                Else
                    Form2.Invoke(Me.ChangeNameProgress, "Finished")
                    Form2.Invoke(Me.ForceBarToFinish)
                End If
            End If
        Else
            MsgBox("An error occured while trying to update CleanCode:" + vbNewLine + vbNewLine + e.Error.Message)
            Form2.Invoke(Me.ChangeNameProgress, "Failed")
            Form2.Invoke(Me.ForceBarToFinish)
        End If
    End Sub

    Shared CurrentFile As String
    Public Sub DownloadProgressChanged(ByVal sender As Object, ByVal e As DownloadProgressChangedEventArgs)
        Form2.Invoke(Me.ChangeName, "Downloading file " + CurrentFile + vbNewLine + e.ProgressPercentage.ToString() + "% (" + e.BytesReceived.ToString() + "b / " + e.TotalBytesToReceive.ToString() + "b)")
    End Sub

    Sub DownloadFile(ByVal file As String, ByVal func As DownloadDataCompletedEventHandler)
        Form2.Invoke(Me.ChangeNameProgress, "Connecting to server...")
        Dim Stripped() As String = file.Split("/")
        CurrentFile = Stripped(Stripped.Length - 1)

        Dim Checker As New WebClient
        AddHandler Checker.DownloadDataCompleted, func
        AddHandler Checker.DownloadProgressChanged, AddressOf DownloadProgressChanged
        Checker.DownloadDataAsync(New Uri(file))
    End Sub

    Sub DoAutoUpdate()
        Form2.Show()
        Form2.Invoke(Me.ChangeNameProgress, "Starting...")
        Form2.WindowState = FormWindowState.Normal

        GetCurrentVersion()

        Me.ShowInTaskbar = False
        Me.WindowState = FormWindowState.Minimized

        DownloadFile("http://cleancodequake2.googlecode.com/svn/trunk/version.ver", AddressOf DoneVersionDownload)
    End Sub

    Private Sub Form1_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load

        ChangeNameProgress = New ChangeTheNameDelegate(AddressOf ChangeTheNameAndProgress)
        ChangeName = New ChangeTheNameDelegate(AddressOf ChangeTheNameOnly)
        ForceBarToFinish = New ForceBarToFinishSub(AddressOf ForceBar)

        TextBox1.Text = Environment.CurrentDirectory()
        If (File.Exists(TextBox1.Text + "\baseq2\gamex86.dll")) Then ' This is a Q2 dir
            DoAutoUpdate()
        End If
    End Sub

    Private Sub Button2_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Button2.Click
        Me.Close()
    End Sub

    Private Sub Button3_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Button3.Click
        Dim Dlg As New FolderBrowserDialog
        Dlg.Description = "Select your Quake II directory"
        Dlg.ShowNewFolderButton = True
        Dlg.ShowDialog()

        TextBox1.Text = Dlg.SelectedPath()
    End Sub

    Private Sub Button1_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Button1.Click
        If (File.Exists(TextBox1.Text + "\baseq2\gamex86.dll")) Then ' This is a Q2 dir
            DoAutoUpdate()
        Else
            MsgBox("This path does not contain a valid Quake II installation!", MsgBoxStyle.OkOnly + MsgBoxStyle.Critical, "CleanCode Auto Updater")
        End If

    End Sub
End Class
