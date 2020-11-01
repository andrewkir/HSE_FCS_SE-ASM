; ������� ������ ����������, ���191
; ������� - 18

; ����������� ���������, ��������� �
; �������� ASCII-������ ������ ������
; ������ ������������������ 5 ��������,
; ������ ������� ������� ������������ ��
; ������� "������ ���������������"

format PE console
entry start

include 'win32a.inc'

;--------------------------------------------------------------------------
section '.data' data readable writable

        startStr     db 'Enter a string longer than 5:', 10,13,0
        wrongSizeStr db 'Wrong length! Repeat with correct length of a string', 10,13,0
        noAnswerStr  db 'Could not find the required substring', 10,13,0
        foundAnswerStr db 'The required substring was found successfully:', 10,13,0
        pressEnterStr  db 'Press Enter to end the program', 10,13,0
        formatS      db '%s', 0     ; ��������� ������ ����������/������
        formatN      db '%d', 0     ; �������� ������ ����������/������
        newLine      db 10, 13, 0   ; ������� ������� �� ����� ������
        pointer      dd ?           ; ��������� �� ����� ����������� �������� � ������
        numCorrect   dd ?           ; ���������� ������ ������ ��������, ��������������� �������
        strLen       dd ?           ; ����� ��������� ������
        addrDiff     dd ?           ; ������� � ������� ����� ������ � ����� �������
        text         db 256 dup(0)  ; ����������� ������
        resultString db 256 dup(0)  ; ���������� ������

;--------------------------------------------------------------------------
section '.code' code readable executable
start:
; ���������� ����������� ������
        cinvoke printf, startStr
        cinvoke scanf, formatS, text

; 1) ���������� � �������� ����� ������
        ccall strlen, text
        mov [strLen], ecx
        mov [pointer], ecx
        cmp [pointer], 5
        jl wrongLen
; ������������ ���������� pointer �������� ������ ����� ������
        mov eax, text
        mov ecx, text
        add ecx, [pointer]
        mov [pointer], ecx
        dec [pointer]
; 2) ������ ��������� �������� �� ������
        xor ecx, ecx
        mov [numCorrect], ecx
        call IterateString
; 3) ��������� ������
        dec [pointer]
        call GetAnswer
; ����� ��������� ���������
        cinvoke printf, foundAnswerStr
        cinvoke printf, resultString
; ����� ������ ���������
        cinvoke printf, newLine
        cinvoke printf, newLine
        cinvoke printf, pressEnterStr
        call [getch]
        push 0
        call [ExitProcess]

; ���������� � ������������ ����� ������ � ������ ���������
wrongLen:
        cinvoke printf, wrongSizeStr
        jmp start

; ��������� ������ ���������
IterateString:
        mov eax, [numCorrect]
        cmp eax, 5
        je endStringLoop       ; ���� ����� 5 ���������� ������ ��������� - ����������� ����

        xor ebx, ebx
        inc ebx
        mov [numCorrect], ebx  ; ����������� �������� numCorrect = 1 (������ ��� ������� ������� ��������)

        mov eax, [pointer]
        cmp eax, text
        jle noAnswer            ; ���� pointer ��������� �� ������ ������, �� ������ ������ �� ���� �������, ��������� � noAnswer
iterate_loop:
        mov eax, [numCorrect]
        cmp eax, 5
        je endStringLoop      ; ���� ����� 5 ���������� ������ ��������� - ����������� ����

        mov eax, [pointer]    ; ��������� � eax pointer (������� �������)
        dec [pointer]
        mov ebx, [pointer]    ; ��������� � ebx pointer (������� ������� �� ���������� �������)

        mov edx, [pointer]
        cmp edx, text
        jl noAnswer            ; ���� pointer ��������� �� ����� �� ������ ������, �� ������ ������ �� ���� �������, ��������� � noAnswer

        mov dl, [eax]
        mov dh, [ebx]
        cmp dl, dh            ; ���������� ��� �������� ������, �������� � �������� ����� ���
        jle IterateString     ; ���� �������� �� ������������� ������� (��������� �������� � ��������������� ���� ���� �����-�������), �� ��������� ������� � IterateString (������������ numCorrect � ����������� ������� �����)

        inc [numCorrect]      ; ���� ������������� �������, �� ����������� ���������� ��������� ��������������� ��������
        jmp iterate_loop
endStringLoop:
        ret

; ��������� ������������ ������
GetAnswer:
        mov eax, resultString
        sub eax, [pointer]
        mov [addrDiff], eax
        dec [addrDiff]        ; ������� �������� ��� ������� � ������� ����� ������ ��������� � text � ������� ������ resultString

        mov ecx, [numCorrect] ; numCorrect = 5, ������ ������ 5 �������� ��� ������������ ������
answer_loop:
        inc [pointer]         ; ��������� � ������������ ���������� �������� � ��������� ������
        mov ebx, [pointer]    ; ebx ��������� �� ������� � ��������� ������ text
        mov eax, [pointer]
        add eax, [addrDiff]   ; eax ��������� �� ������� � ������ resultString
        mov dl, [ebx]         ; ����� ������� �� ��������� ������
        mov [eax], dl         ; � ���������� ��� � �����
        loop answer_loop
endAnsweLoop:
        mov eax, resultString
        add eax, 5            ; �������� ����� ���������� �������� ������ resultString
        mov byte [eax], 0     ; ���������� � ���� ���� ����� ������
        ret

; ���������� � ������������� ����� ��������� � ���������� ���������
noAnswer:
        cinvoke printf, noAnswerStr
        cinvoke printf, newLine
        cinvoke printf, pressEnterStr
        call [getch]
        push 0
        call [ExitProcess]

; ������� ��������� ����� ������
proc  strlen, strInput
        mov ecx,-1
        mov al,0    ; ������� � ������� al ���� ����� ������
        mov edi,[strInput]
        cld         ; ����� ����������� ������ ������
        repne scasb ; ���������� ������ ���� ������ �� ��������� � ������� al, ��������� ������������ � ecx
        not ecx     ; ��� ��� ���� ���������� � ������ ��������� ������, ������� ���� ����� �� ecx
        dec ecx
        ret
endp
;-------------------------------third act - including HeapApi--------------------------
                                                 
section '.idata' import data readable
    library kernel, 'kernel32.dll',\
            msvcrt, 'msvcrt.dll',\
            user32,'USER32.DLL'

include 'api\user32.inc'
include 'api\kernel32.inc'
    import kernel,\
           ExitProcess, 'ExitProcess',\
           HeapCreate,'HeapCreate',\
           HeapAlloc,'HeapAlloc'
  include 'api\kernel32.inc'
    import msvcrt,\
           printf, 'printf',\
           scanf, 'scanf',\
           getch, '_getch',\
           gets, 'gets'