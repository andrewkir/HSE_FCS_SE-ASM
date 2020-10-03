; Кирюхин Андрей БПИ191, Вариант 18

; условие задачи:
; Разработать программу, которая вводит одномерный массив A[N],
; формирует из элементов массива A новый массив B по правилам, указанным в таблице, и выводит его.
; Составить массив B с уменьшением всех элементов до первого положительного на 5.

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
; 1) ввод вектора
        call VectorInput
; 2) создание нового вектора, по правилам описанным в условии задачи
        call VectorSubstract
; 3) вывод результата
        push strOriginVector
        call [printf]
        add esp, 4
; вывод начального вектора
        mov ebx, vec
        call VectorOut

        push strSubsVector
        call [printf]
        add esp, 4
; вывод нового вектора
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

; проверка введенного размера, если <= 0, то повторяем VectorInput
        cmp [vec_size], 0
        jg  getVector

        push [vec_size]
        push strIncorSize
        call [printf]
        jmp VectorInput
; если число подходит, переходим в getVector
getVector:
        xor ecx, ecx            ; ecx = 0
        mov ebx, vec            ; ebx = &vec
getVecLoop:
        mov [tmp], ebx
        cmp ecx, [vec_size]
        jge endInputVector       ; если массив полностью заполнен, то переходим в endInputVector

        ; ввод элемента
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
        xor ecx, ecx            ; ecx = 0 счетчик
        mov ebx, vec            ; ebx = &vec работа с изначальным вектором
        mov edx, vecNew         ; edx = &vecNew работа с новым вектором
subsVecLoop:
        cmp ecx, [vec_size]
        je endSubstractVector      ; если дошли до конца вектора, переходим в endSubstractVector

; если число больше нуля, то изменяем переменную flag, переходим в markFlagAddNumber
        mov eax, [ebx]
        cmp eax, 0
        jg markFlagAddNumber

; иначе если число меньше нуля, но переменная flag != 0, то переходим в markFlagAddNumber (для добавления текущего элемента)
        mov eax, flag
        cmp eax, 0
        jg markFlagAddNumber

; добавляем элемент в новый вектор и вычитаем из него 5
        mov eax, [ebx]
        add eax, -5
        mov [edx], eax
        add edx, 4

; увеличиваем счетчик
incPointer:
        inc ecx
        add ebx, 4
        jmp subsVecLoop

markFlagAddNumber:
; добавляем нетронутый элемент из vec, так как произошел переход к этой части кода, без его добавления
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
        je endOutputVector      ; если дошли до конца вектора, переходим в endOutputVector
        mov [i], ecx

        ; вывод одного элемента
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