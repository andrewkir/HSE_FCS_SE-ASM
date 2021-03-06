; ������� ������ ���191, ������� 18

; ������� ������:
; ����������� ���������, ������� ������ ���������� ������ A[N],
; ��������� �� ��������� ������� A ����� ������ B �� ��������, ��������� � �������, � ������� ���.
; ��������� ������ B � ����������� ���� ��������� �� ������� �������������� �� 5.

format PE console
entry start

include 'win32a.inc'

;--------------------------------------------------------------------------
section '.data' data readable writable

        strVecSize   db 'Enter size of the vector = ', 0
        strIncorSize db 'Incorrect size of the vector = %d, size must be greater than zero', 10, 0
        strVecElemI  db '[%d] = ', 0
        strScanInt   db '%d', 0
        strVecElemOut  db '[%d] = %d', 10, 0
        strOriginVector db 'Original vector:',10,0
        strSubsVector db 'Changed vector:',10,0

        vec_size     dd 0
        i            dd ?
        tmp          dd ?
        tmpStack     dd ?
        vec          rd 100
        vecNew       rd 100
        flag = 0

;--------------------------------------------------------------------------
section '.code' code readable executable
start:
; 1) ���� �������
        call VectorInput
; 2) �������� ������ �������, �� �������� ��������� � ������� ������
        call VectorSubstract
; 3) ����� ����������
        push strOriginVector
        call [printf]
        add esp, 4
; ����� ���������� �������
        mov ebx, vec
        call VectorOut

        push strSubsVector
        call [printf]
        add esp, 4
; ����� ������ �������
        mov ebx, vecNew
        call VectorOut
finish:
        call [getch]

        push 0
        call [ExitProcess]

;--------------------------------------------------------------------------
VectorInput:
        push strVecSize
        call [printf]
        add esp, 4

        push vec_size
        push strScanInt
        call [scanf]
        add esp, 8

; �������� ���������� �������, ���� <= 0, �� ��������� VectorInput
        cmp [vec_size], 0
        jg  getVector

        push [vec_size]
        push strIncorSize
        call [printf]
        jmp VectorInput
; ���� ����� ��������, ��������� � getVector
getVector:
        xor ecx, ecx            ; ecx = 0
        mov ebx, vec            ; ebx = &vec
getVecLoop:
        mov [tmp], ebx
        cmp ecx, [vec_size]
        jge endInputVector       ; ���� ������ ��������� ��������, �� ��������� � endInputVector

        ; ���� ��������
        mov [i], ecx
        push ecx
        push strVecElemI
        call [printf]
        add esp, 8

        push ebx
        push strScanInt
        call [scanf]
        add esp, 8

        mov ecx, [i]
        inc ecx
        mov ebx, [tmp]
        add ebx, 4
        jmp getVecLoop
endInputVector:
        ret
;--------------------------------------------------------------------------
VectorSubstract:
        xor ecx, ecx            ; ecx = 0 �������
        mov ebx, vec            ; ebx = &vec ������ � ����������� ��������
        mov edx, vecNew         ; edx = &vecNew ������ � ����� ��������
subsVecLoop:
        cmp ecx, [vec_size]
        je endSubstractVector      ; ���� ����� �� ����� �������, ��������� � endSubstractVector

; ���� ����� ������ ����, �� �������� ���������� flag, ��������� � markFlagAddNumber
        mov eax, [ebx]
        cmp eax, 0
        jg markFlagAddNumber

; ����� ���� ����� ������ ����, �� ���������� flag != 0, �� ��������� � markFlagAddNumber (��� ���������� �������� ��������)
        mov eax, flag
        cmp eax, 0
        jg markFlagAddNumber

; ��������� ������� � ����� ������ � �������� �� ���� 5
        mov eax, [ebx]
        add eax, -5
        mov [edx], eax
        add edx, 4

; ����������� �������
incPointer:
        inc ecx
        add ebx, 4
        jmp subsVecLoop

markFlagAddNumber:
; ��������� ���������� ������� �� vec, ��� ��� ��������� ������� � ���� ����� ����, ��� ��� ����������
        flag = 1
        mov eax, [ebx]
        mov [edx], eax
        add edx, 4
        jmp incPointer

endSubstractVector:
        ret

;--------------------------------------------------------------------------
VectorOut:
        mov [tmpStack], esp
        xor ecx, ecx            ; ecx = 0
putVecLoop:
        mov [tmp], ebx
        cmp ecx, [vec_size]
        je endOutputVector      ; ���� ����� �� ����� �������, ��������� � endOutputVector
        mov [i], ecx

        ; ����� ������ ��������
        push dword [ebx]
        push ecx
        push strVecElemOut
        call [printf]

        mov ecx, [i]
        inc ecx
        mov ebx, [tmp]
        add ebx, 4
        jmp putVecLoop
endOutputVector:
        mov esp, [tmpStack]
        ret
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
           getch, '_getch'