Public Class Form1

    Structure Entity
        Public Name As String
        Public ClassName As String
        Public HasFields As Boolean
    End Structure

    Public EntityTypeArray() As Entity = { _
    New Entity With {.Name = "Map Entity", .ClassName = "CMapEntity", .HasFields = True}, _
    New Entity With {.Name = "Hurtable", .ClassName = "CHurtableEntity", .HasFields = True}, _
    New Entity With {.Name = "Blockable", .ClassName = "CBlockableEntity", .HasFields = False}, _
    New Entity With {.Name = "Thinkable", .ClassName = "CThinkableEntity", .HasFields = False}, _
    New Entity With {.Name = "Touchable", .ClassName = "CTouchableEntity", .HasFields = False}, _
    New Entity With {.Name = "Usable", .ClassName = "CUsableEntity", .HasFields = True}, _
    New Entity With {.Name = "Base Physics", .ClassName = "CPhysicsEntity", .HasFields = False}, _
    New Entity With {.Name = "Bounce Physics", .ClassName = "CBounceProjectile", .HasFields = False}, _
    New Entity With {.Name = "Toss Physics", .ClassName = "CTossProjectile", .HasFields = False}, _
    New Entity With {.Name = "FlyMissile Physics", .ClassName = "CFlyMissileProjectile", .HasFields = False}, _
    New Entity With {.Name = "Step Physics", .ClassName = "CStepPhysics", .HasFields = False}, _
    New Entity With {.Name = "Push Physics", .ClassName = "CPushPhysics", .HasFields = False}, _
    New Entity With {.Name = "Stop Physics", .ClassName = "CStopPhysics", .HasFields = False}, _
    New Entity With {.Name = "Junk Entity", .ClassName = "CJunkEntity", .HasFields = False}, _
    New Entity With {.Name = "Brush Model", .ClassName = "CBrushModel", .HasFields = True} _
    }

    Structure DataType
        Public Name As String
        Public Type As String
        Public DefaultValue As String
        Public EnumName As String
        Public Pointer As Boolean

        Sub New(ByRef _Name As String, ByRef _Type As String, ByRef _DefaultValue As String, ByRef _EnumName As String, Optional ByRef _Pointer As Boolean = False)
            Name = _Name
            Type = _Type
            DefaultValue = _DefaultValue
            EnumName = _EnumName
            Pointer = _Pointer
        End Sub
    End Structure

    Public DataTypeArray() As DataType = { _
    New DataType("Ignore", Nothing, Nothing, "FT_IGNORE"), _
    New DataType("Integer", "int", "0", "FT_INT"), _
    New DataType("UInteger", "uint32", "0", "FT_UINT"), _
    New DataType("Float", "float", "0.0f", "FT_FLOAT"), _
    New DataType("Vector", "vec3f", "", "FT_VECTOR"), _
    New DataType("Angle (yaw only)", "vec3f", "", "FT_YAWANGLE"), _
    New DataType("Level String", "char", "NULL", "FT_LEVEL_STRING", True), _
    New DataType("Game String", "char", "NULL", "FT_GAME_STRING", True), _
    New DataType("Sound index", "MediaIndex", "0", "FT_SOUND_INDEX"), _
    New DataType("Image index", "MediaIndex", "0", "FT_IMAGE_INDEX"), _
    New DataType("Model index", "MediaIndex", "0", "FT_MODEL_INDEX"), _
    New DataType("Frame/Time", "FrameNumber_t", "0", "FT_FRAMENUMBER"), _
    New DataType("Item", "CBaseItem", "NULL", "FT_ITEM", True), _
    New DataType("Entity", "CBaseEntity", "NULL", "FT_ENTITY", True), _
    New DataType("Float to Byte", "byte", "0", "FT_FLOAT_TO_BYTE") _
    }

    Public Function DataTypeFromString(ByRef name As String) As DataType
        For Each Dat In DataTypeArray
            If Dat.Name = name Then Return Dat
        Next
        Return Nothing
    End Function

    Public Sub WriteVariables(ByRef WrittenClass As String)
        Dim Count As Integer = 0
        For Each Row As DataGridViewRow In DataGridView1.Rows
            If (Row.Cells(0).EditedFormattedValue() Is "" Or Row.Cells(1).EditedFormattedValue() Is "" Or Row.Cells(2).EditedFormattedValue() Is "") Then Exit For

            If (DataTypeFromString(Row.Cells(2).EditedFormattedValue()).Name = DataTypeArray(0).Name) Then
                Continue For
            End If

            Count = Count + 1
            WrittenClass = WrittenClass + "\t" + DataTypeFromString(Row.Cells(2).EditedFormattedValue()).Type + " "
            If DataTypeFromString(Row.Cells(2).EditedFormattedValue()).Pointer = True Then
                WrittenClass = WrittenClass + "*"
            End If
            WrittenClass = WrittenClass + Row.Cells(1).EditedFormattedValue().ToString() + ";\n"
        Next

        If (Count > 0) Then
            WrittenClass = WrittenClass + "\n"
        End If
    End Sub

    Public Sub WriteFields(ByVal InsideClass As Boolean, ByRef WrittenClass As String, ByRef List As ListBox, ByRef Selected As ListBox.SelectedIndexCollection)
        If Selected.Contains(0) <> True Then Return

        Dim HasCustomFields As Boolean = CheckBox3.Checked
        Dim CallOthers As Boolean = CheckBox4.Checked
        Dim VirtualFields As Boolean = CheckBox5.Checked

        If InsideClass = True Then

            If HasCustomFields Then
                ' Simple; write the correct macro.
                If VirtualFields Then
                    WrittenClass = WrittenClass + "\tENTITYFIELD_VIRTUAL_DEFS\n\n"
                Else
                    WrittenClass = WrittenClass + "\tENTITYFIELD_DEFS\n\n"
                End If
            Else
                ' No fields; just write ParseField
                WrittenClass = WrittenClass + "\t"
                If VirtualFields Then WrittenClass = WrittenClass + "virtual "
                WrittenClass = WrittenClass + "bool ParseField (const char *Key, const char *Value)\n\t{\n"

                ' Inside
                If CallOthers Then
                    Dim FieldIndexes As Collection = New Collection
                    For Each Index In Selected
                        If EntityTypeArray(Index).HasFields Then FieldIndexes.Add(Index)
                    Next

                    If (FieldIndexes.Count() = 1) Then ' Only CMapEntity
                        WrittenClass = WrittenClass + "\t\treturn CMapEntity::ParseField(Key, Value);"
                    Else
                        WrittenClass = WrittenClass + "\t\treturn ("
                        Dim i As Integer = 0
                        For Each Index In FieldIndexes

                            If (i <> 0) Then WrittenClass = WrittenClass + " || "
                            WrittenClass = WrittenClass + EntityTypeArray(Index).ClassName + "::ParseField (Key, Value)"
                            i = i + 1
                        Next
                        WrittenClass = WrittenClass + ");"
                    End If
                Else
                    WrittenClass = WrittenClass + "\t\treturn false;"
                End If

                ' Done
                WrittenClass = WrittenClass + "\n\t}\n\n"
            End If
        Else
            If HasCustomFields Then
                ' Only do this if we have custom fields.
                ' Write a basic skeleton code for the fields first.

            End If
        End If
    End Sub

    Public Sub InsertComment(ByRef WrittenClass As String, ByVal BasicComment As String, ByVal AdvancedComment As String)
        If RadioButton1.Checked Then
            Return
        ElseIf RadioButton2.Checked Then
            If BasicComment = Nothing Then Return

            WrittenClass = WrittenClass + BasicComment
        ElseIf RadioButton3.Checked Then
            If AdvancedComment = Nothing Then
                If BasicComment = Nothing Then Return

                WrittenClass = WrittenClass + BasicComment
                Return
            End If

            WrittenClass = WrittenClass + AdvancedComment
        End If
    End Sub

    Public Function CreateClass() As String
        ' Create the class
        Dim List As ListBox = ListBox1
        Dim Selected As ListBox.SelectedIndexCollection = List.SelectedIndices
        Dim ClassCode As Boolean = CheckBox1.Checked
        Dim WrittenClass As String

        WrittenClass = "class " + TextBox1.Text
        'If (CheckBox2.Checked) Then
        'WrittenClass = WrittenClass + " virtual"
        'End If

        WrittenClass = WrittenClass + " : "

        ' None selected, just throw in CBaseEntity
        If Selected.Count = 0 Then
            WrittenClass = WrittenClass + "public virtual CBaseEntity"
        Else
            Dim CountOfClasses As Integer = 0
            ' Put each sequential class after each other
            For i As Integer = 0 To EntityTypeArray.Length - 1
                If Selected.Contains(i) Then
                    If CountOfClasses = 0 Then
                        WrittenClass = WrittenClass + "public " + EntityTypeArray(i).ClassName
                    Else
                        WrittenClass = WrittenClass + ", public " + EntityTypeArray(i).ClassName
                    End If
                    CountOfClasses = CountOfClasses + 1
                End If
            Next
        End If

        ' Constructor area
        WrittenClass = WrittenClass + "\n{\npublic:\n"

        InsertComment(WrittenClass, Nothing, "\t// Variables\n")
        WriteVariables(WrittenClass)

        InsertComment(WrittenClass, "\t// Constructors\n", "\t// Constructor for creating a new entity\n")
        WrittenClass = WrittenClass + "\t" + TextBox1.Text + " ()"

        If (ClassCode <> True) Then
            WrittenClass = WrittenClass + ";\n\t"
        Else
            WrittenClass = WrittenClass + " :\n\t  CBaseEntity ()"

            If (Selected.Count <> 0) Then
                For Each Blah In Selected
                    WrittenClass = WrittenClass + ",\n\t  " + EntityTypeArray(Blah).ClassName + " ()"
                Next
            End If
            WrittenClass = WrittenClass + "\n\t{\n\t};\n\n"
        End If

        InsertComment(WrittenClass, Nothing, "\t//Constructor for re-using an existing entity\n")
        WrittenClass = WrittenClass + "\t" + TextBox1.Text + " (int Index)"

        If (ClassCode <> True) Then
            WrittenClass = WrittenClass + ";\n\n"
        Else
            WrittenClass = WrittenClass + " :\n\t  CBaseEntity (Index)"

            If (Selected.Count <> 0) Then
                For Each Blah In Selected
                    WrittenClass = WrittenClass + ",\n\t  " + EntityTypeArray(Blah).ClassName + " (Index)"
                Next
            End If
            WrittenClass = WrittenClass + "\n\t{\n\t};\n\n"
        End If

        WriteFields(True, WrittenClass, List, Selected)

        ' Functions that must get overrode
        ' Only show Run if we'd have a dominance war
        InsertComment(WrittenClass, "\t// Called to run the entity\n", "\t// Called to run the entity\n\t// Note that if you inherited certain classes you will\n\t// need to modify this accordingly.\n")
        If Selected.Count > 0 Then
            WrittenClass = WrittenClass + "\tbool Run ()"
            If (ClassCode <> True) Then
                WrittenClass = WrittenClass + ";"
            Else
                WrittenClass = WrittenClass + "\n\t{\n\t\treturn CBaseEntity::Run();\n\t};"
            End If
        End If

        ' Seperator
        WrittenClass = WrittenClass + "\n\n"

        ' Rest of the functions that must get overrode
        Dim co As Integer = 0
        For Each Num In Selected
            Dim WroteSomething As Boolean = False
            If Num = 0 Then Continue For ' Do mapentity last

            If Num = 1 Then
                If (ClassCode <> True) Then
                    WroteSomething = True

                    InsertComment(WrittenClass, "\t// Called when the entity is damaged\n", Nothing)
                    WrittenClass = WrittenClass + "\tvoid Pain (CBaseEntity *other, float kick, int damage);\n\n"

                    InsertComment(WrittenClass, "\t// Called when the entity is damaged and it's health\n\t// is below or equal to 0\n", Nothing)
                    WrittenClass = WrittenClass + "\tvoid Die (CBaseEntity *inflictor, CBaseEntity *attacker, int damage, vec3f &point);\n"
                Else
                    WroteSomething = True

                    InsertComment(WrittenClass, "\t// Called when the entity is damaged\n", Nothing)
                    WrittenClass = WrittenClass + "\tvoid Pain (CBaseEntity *other, float kick, int damage)\n\t{\n\t};\n\n"

                    InsertComment(WrittenClass, "\t// Called when the entity is damaged and it's health\n\t// is below or equal to 0\n", Nothing)
                    WrittenClass = WrittenClass + "\tvoid Die (CBaseEntity *inflictor, CBaseEntity *attacker, int damage, vec3f &point)\n\t{\n\t};\n"
                End If
            ElseIf Num = 2 Then
                If (ClassCode <> True) Then
                    WroteSomething = True

                    InsertComment(WrittenClass, "\t// Called when the entity is blocked by a push physics object\n", Nothing)
                    WrittenClass = WrittenClass + "\tvoid Blocked (CBaseEntity *other);\n"
                Else
                    WroteSomething = True

                    InsertComment(WrittenClass, "\t// Called when the entity is blocked by a push physics object\n", Nothing)
                    WrittenClass = WrittenClass + "\tvoid Blocked (CBaseEntity *other)\n\t{\n\t};\n"
                End If
            ElseIf Num = 3 Then
                If Selected.Contains(14) <> True Then
                    If (ClassCode <> True) Then
                        WroteSomething = True

                        InsertComment(WrittenClass, "\t// Called when the entity's NextThink has reached current frame\n", Nothing)
                        WrittenClass = WrittenClass + "\tvoid Think ();\n"
                    Else
                        WroteSomething = True

                        InsertComment(WrittenClass, "\t// Called when the entity's NextThink has reached current frame\n", Nothing)
                        WrittenClass = WrittenClass + "\tvoid Think ()\n\t{\n\t};\n"
                    End If
                End If
            ElseIf Num = 4 Then
                If (ClassCode <> True) Then
                    WroteSomething = True

                    InsertComment(WrittenClass, "\t// Called when the entity collides\n", Nothing)
                    WrittenClass = WrittenClass + "\tvoid Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf);\n"
                Else
                    WroteSomething = True

                    InsertComment(WrittenClass, "\t// Called when the entity collides\n", Nothing)
                    WrittenClass = WrittenClass + "\tvoid Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf)\n\t{\n\t};\n"
                End If
            ElseIf Num = 5 Then
                If (ClassCode <> True) Then
                    WroteSomething = True

                    InsertComment(WrittenClass, "\t// Called when the entity is used by another entity\n", Nothing)
                    WrittenClass = WrittenClass + "\tvoid Use (CBaseEntity *other, CBaseEntity *activator);\n"
                Else
                    WroteSomething = True

                    InsertComment(WrittenClass, "\t// Called when the entity is used by another entity\n", Nothing)
                    WrittenClass = WrittenClass + "\tvoid Use (CBaseEntity *other, CBaseEntity *activator)\n\t{\n\t};\n"
                End If
                ' Physics don't have any

            ElseIf Num = 14 Then
                If (ClassCode <> True) Then
                    WroteSomething = True

                    InsertComment(WrittenClass, "\t// Called when the entity is done it's movement\n", Nothing)
                    WrittenClass = WrittenClass + "\tvoid DoEndFunc ();\n\n\tvoid Think ();\n"
                Else
                    WroteSomething = True

                    InsertComment(WrittenClass, "\t// Called when the entity is done it's movement\n", Nothing)
                    WrittenClass = WrittenClass + "\tvoid DoEndFunc ()\n\t{\n\t};\n\n\tvoid Think ()\n\t{\n\t};\n"
                End If
            End If

            ' Seperator
            co = co + 1
            If WroteSomething And co <> Selected.Count - 1 Then
                WrittenClass = WrittenClass + "\n"
            End If
        Next

        If co <> 0 And WrittenClass.EndsWith("\n\n") = False Then
            WrittenClass = WrittenClass + "\n"
        End If

        If Selected.Contains(0) Then
            InsertComment(WrittenClass, "\t// Called when the entity gets spawned\n", Nothing)
            If (ClassCode <> True) Then
                WrittenClass = WrittenClass + "\tvoid Spawn ();\n"
            Else
                WrittenClass = WrittenClass + "\tvoid Spawn ()\n\t{\n\t};\n"
            End If
        End If

        ' End of class
        WrittenClass = WrittenClass + "};"

        ' If we didn't specify code-in-class we'll write the class code here
        If ClassCode <> True Then
            ' Constructors
            WrittenClass = WrittenClass + "\n\n" + TextBox1.Text + "::" + TextBox1.Text + " ()" + " :\n\tCBaseEntity ()"

            If (Selected.Count <> 0) Then
                For Each Blah In Selected
                    WrittenClass = WrittenClass + ",\n\t" + EntityTypeArray(Blah).ClassName + " ()"
                Next
            End If

            WrittenClass = WrittenClass + "\n{\n};\n\n" + TextBox1.Text + "::" + TextBox1.Text + " (int Index)" + " :\n\tCBaseEntity (Index)"

            If (Selected.Count <> 0) Then
                For Each Blah In Selected
                    WrittenClass = WrittenClass + ",\n\t" + EntityTypeArray(Blah).ClassName + " (Index)"
                Next
            End If
            WrittenClass = WrittenClass + "\n{\n};\n\n"

            ' Functions
            co = 0
            For Each Num In Selected
                Dim WroteSomething As Boolean = False
                If Num = 0 Then Continue For ' Do mapentity last

                If Num = 1 Then
                    WrittenClass = WrittenClass + "void " + TextBox1.Text + "::" + "Pain (CBaseEntity *other, float kick, int damage)\n{\n};\n\nvoid " + TextBox1.Text + "::" + "Die (CBaseEntity *inflictor, CBaseEntity *attacker, int damage, vec3_t point)\n{\n};\n"
                    WroteSomething = True
                ElseIf Num = 2 Then
                    WrittenClass = WrittenClass + "void " + TextBox1.Text + "::" + "Blocked (CBaseEntity *other)\n{\n};\n"
                    WroteSomething = True
                ElseIf Num = 3 Then
                    If Selected.Contains(14) <> True Then
                        WrittenClass = WrittenClass + "void " + TextBox1.Text + "::" + "Think ()\n{\n};\n"
                        WroteSomething = True
                    End If
                ElseIf Num = 4 Then
                    WroteSomething = True
                    WrittenClass = WrittenClass + "void " + TextBox1.Text + "::" + "Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf)\n{\n};\n"
                ElseIf Num = 5 Then
                    WrittenClass = WrittenClass + "void " + TextBox1.Text + "::" + "Use (CBaseEntity *other, CBaseEntity *activator)\n{\n};\n"
                    WroteSomething = True
                    ' Physics don't have any
                ElseIf Num = 14 Then
                    WroteSomething = True
                    WrittenClass = WrittenClass + "void " + TextBox1.Text + "::" + "DoEndFunc ()\n{\n};\n\nvoid " + TextBox1.Text + "::" + "Think ()\n{\n};\n"
                End If

                ' Seperator
                co = co + 1
                If WroteSomething And co <> Selected.Count - 1 Then
                    WrittenClass = WrittenClass + "\n"
                End If
            Next

            If Selected.Contains(0) Then
                If WrittenClass.EndsWith("\n\n") = False Then WrittenClass = WrittenClass + "\n"
                WrittenClass = WrittenClass + "void " + TextBox1.Text + "::" + "Spawn ()\n{\n};\n"
            End If
        End If

        ' Do we have fields to write?
        If DataGridView1.RowCount Then
            ' Were we proceeded by two newlines?
            If WrittenClass.EndsWith("\n") = True Then
                If WrittenClass.EndsWith("\n\n") = False Then
                    WrittenClass = WrittenClass + "\n"
                End If
            Else
                WrittenClass = WrittenClass + "\n\n"
            End If

            ' Write the header portion
            WrittenClass = WrittenClass + "ENTITYFIELDS_BEGIN(" + TextBox1.Text + ")\n{"

            ' Write fields
            For Each Row As DataGridViewRow In DataGridView1.Rows
                If (Row.Cells(0).EditedFormattedValue() Is "" Or Row.Cells(1).EditedFormattedValue() Is "" Or Row.Cells(2).EditedFormattedValue() Is "") Then Exit For
                Dim Type As DataType = DataTypeFromString(Row.Cells(2).EditedFormattedValue())

                Dim ExtraFlags As String = ""
                If CType(Row.Cells(3), DataGridViewCheckBoxCell).EditingCellFormattedValue = True Then
                    ExtraFlags = ExtraFlags + " | FT_SAVABLE"
                End If
                If CType(Row.Cells(4), DataGridViewCheckBoxCell).EditingCellFormattedValue = True Then
                    ExtraFlags = ExtraFlags + " | FT_NOSPAWN"
                End If

                WrittenClass = WrittenClass + "\n\tCEntityField (""" + Row.Cells(0).EditedFormattedValue().ToString() + """, EntityMemberOffset(" + TextBox1.Text + "," + Row.Cells(1).EditedFormattedValue().ToString() + "), " + Type.EnumName + ExtraFlags + "),"
            Next

            WrittenClass = WrittenClass + "\n};\nENTITYFIELDS_END(" + TextBox1.Text + ")\n\n"
        End If

        If Selected.Contains(0) Then
            WrittenClass = WrittenClass + "LINK_CLASSNAME_TO_CLASS (""" + TextBox2.Text + """, " + TextBox1.Text + ");"
        End If

        WrittenClass = WrittenClass.Replace("\n", vbNewLine)
        WrittenClass = WrittenClass.Replace("\t", vbTab)

        Return WrittenClass
    End Function

    Private Sub Form1_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        TabControl1.TabPages.Remove(TabPage2)
        TabControl1.TabPages.Remove(TabPage3)

        For Each HolyStr In EntityTypeArray
            ListBox1.Items.Add(HolyStr.Name)
        Next

        For Each Stri In DataTypeArray
            Column3.Items.Add(Stri.Name)
        Next
    End Sub

    Private Sub ExitToolStripMenuItem_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ExitToolStripMenuItem.Click
        Me.Close()
    End Sub

    Private Sub ListBox1_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ListBox1.SelectedIndexChanged
        If ListBox1.SelectedIndices.Contains(0) And TabControl1.TabPages.Contains(TabPage3) <> True Then
            TabControl1.TabPages.Add(TabPage3)
        ElseIf ListBox1.SelectedIndices.Contains(0) = False And TabControl1.TabPages.Contains(TabPage3) Then
            TabControl1.TabPages.Remove(TabPage3)

            If TabControl1.TabPages.Contains(TabPage2) Then TabControl1.TabPages.Remove(TabPage2)
        End If
    End Sub

    Private Sub CheckBox3_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles CheckBox3.CheckedChanged
        If CheckBox3.Checked Then
            TabControl1.TabPages.Add(TabPage2)
        Else
            TabControl1.TabPages.Remove(TabPage2)
        End If
    End Sub

    Private Sub CopyToClipboardToolStripMenuItem_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles CopyToClipboardToolStripMenuItem.Click
        Clipboard.SetText(CreateClass())
    End Sub
End Class
