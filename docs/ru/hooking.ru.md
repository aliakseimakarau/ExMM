﻿# Схема перехвата

Для перехвата обращений к регистровому файлу моделируемого устройства используется механизм защиты памяти, реалиуемый в операционных системах Windows и Linux. В настоящий момент поддерживаются архитектуры x86 и x86_64.

Контроллер памяти (MMU) поддерживает атрибуты контроля доступа к страницам виртуальной памяти: чтение, запись, выполнение в различных комбинациях, в том числе полный запрет доступа к страницам. При некорректном обращении к памяти MMU генерирует исключительную ситуацию, которую операционная система может обработать по собственному усмотрению: уничтожить процесс, выделить страницу физической памяти иил преобразовать исключительную ситуацию в исключение (SEH в Windows) или POSIX-сигнал.
В случае возникновения исключения/сигнала процесс может обработать такую ситуацию, выполнив некоторые действия, и повторить попытку выполнения кода с той же инструкции.

## Перехват записи

При записи значения в регистровый файл происходит ошибка защиты памяти, что приводит к следующей последовательности действий:
- вызывается обработчик ошибки доступа (SIGSEGV или EXCEPTION_ACCESS_VIOLATION), который на основе машинного контекста определяет, что была попытка записи в память, а также выполняет поиск в реестре контроллера, который управляет областью памяти, доступ к которой был перехвачен;
- через машинный контекст включается пошаговое выполнение программы включением бита TF регистра EFLAGS в машинном контексте (только для x86/x86_64);
- снимается защита памяти регистрового файла (устанавливается режим защиты чтение/запись);
- перезапускается выполнение программы с той же инструкции: инструкция записи, вызвавшая исключение, будет выполнена успешно;
- возникает исключительная ситуация трассировки программы, и вызывается обработчик события трассировки (SIGTRAP или EXCEPTION_SINGLE_STEP)
- снимается флаг трассировки в машинном контексте;
- вызывается метод перехвата записи контроллера (защита памяти всё ещё снята);
- восстанавливается исходная защита памяти;
- продолжается выполнение программы.

## Перехват чтения

При чтении значения из регистрового файла, если перехват чтения разрешён, происходит ошибка защиты памяти, что приводит к следующей последовательности действий:
- вызывается обработчик ошибки доступа (SIGSEGV или EXCEPTION_ACCESS_VIOLATION), который на основе машинного контекста определяет, что была попытка чтения из памяти, а также выполняет поиск в реестре контроллера, который управляет областью памяти, доступ к которой был перехвачен;
- снимается защита памяти регистрового файла (устанавливается режим защиты чтение/запись);
- вызывается метод перехвата операции чтения контроллера;
- через машинный контекст включается пошаговое выполнение программы включением бита TF регистра EFLAGS в машинном контексте (только для x86/x86_64);
- перезапускается выполнение программы с той же инструкции: инструкция чтения, вызвавшая исключение, будет выполнена успешно;
- возникает исключительная ситуация трассировки программы, и вызывается обработчик события трассировки (SIGTRAP или EXCEPTION_SINGLE_STEP)
- снимается флаг трассировки в машинном контексте;
- восстанавливается исходная защита памяти;
- продолжается выполнение программы.
