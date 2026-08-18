# COLogger

*[Read this in English](README.md)*

Es una pequeña librería de logging para C++20 en Windows, construida alrededor de una pequeña API de macros y un sistema de formato de strings type-safe, sin `printf`.

COLogger permite que una aplicación registre mensajes en una consola con colores, en un archivo de log (append-only), o en ambos — con niveles de log, eliminación de verbosidad en tiempo de compilación, y formateo de valores inline (incluyendo hexadecimal) — sin depender de un framework de logging pesado.

El objetivo de este proyecto fue entender cómo funciona la consola de Windows y cómo construir un logger ligero desde cero.

## Características

- **Salidas a consola y archivo**, habilitables de forma independiente (`InitLogSystem(useConsole, logToFile)`)
- **Salida de consola coloreada** mediante códigos de escape ANSI/VT100, un color por nivel de log
- **Cinco niveles de log**: `Verbose`, `Log`, `Warning`, `Error`, `Critical`
- **Formateo inline type-safe** — sustituye valores dentro de un mensaje sin usar especificadores de formato al estilo `printf`, con formateo hexadecimal opcional por argumento
- **Eliminación de niveles de log en tiempo de compilación** — deshabilita niveles de log verbosos/ruidosos por completo en builds de release con un único define de preprocesador, sin costo en tiempo de ejecución
- **Ruta de exportación con ABI de C** para consumir el logger desde C, C#, o cualquier lenguaje que pueda invocar una DLL de Windows

## Inicio rápido

```cpp
#include "OutputLogger.h"

int main()
{
    // Registrar tanto en consola como en output.log
    InitLogSystem(true, true);

    OLOG_L("Application started");
    OLOG_WF("Player {0} disconnected after {1} seconds", playerName, sessionSeconds);
    OLOG_EF("Failed with error code {#0}", errorCode); // {#0} = imprimir en hexadecimal

    OLOG_CLEAR(); // limpiar la consola
}
```

Ejemplo de salida por consola:

```
[2026-08-17 14:02:11] Log: Application started
[2026-08-17 14:02:13] Warning: Player Alex disconnected after 42 seconds
[2026-08-17 14:02:15] Error: Failed with error code 0x1a
```

## Referencia de la API

| Macro | Descripción |
|---|---|
| `InitLogSystem(useConsole, logToFile)` | Inicializa la instancia global del logger. Se llama una única vez al iniciar. |
| `OLOG(level, message)` | Registra un mensaje en un `OLoggerLevel` arbitrario. |
| `OLOG_V` / `OLOG_L` / `OLOG_W` / `OLOG_E` / `OLOG_C` | Atajos de `OLOG` para Verbose / Log / Warning / Error / Critical. |
| `OFORMAT(message, ...)` | Construye un string formateado a partir de una plantilla y argumentos (ver más abajo). Se usa de forma independiente o dentro de las macros `*_F`. |
| `OLOG_VF` / `OLOG_LF` / `OLOG_WF` / `OLOG_EF` / `OLOG_CF` | Combina `OLOG` + `OFORMAT` para cada nivel. |
| `OLOG_CLEAR()` | Limpia la consola. |

### Sintaxis del formato de strings

`OFORMAT` (y las macros de log `*_F`) toman una plantilla de mensaje con placeholders `{N}`, sustituidos en el orden de los argumentos:

```cpp
OFORMAT("{0} + {1} = {2}", a, b, a + b);
```

Anteponé `#` al índice de un placeholder para que ese argumento se muestre en hexadecimal en lugar de decimal:

```cpp
OFORMAT("Pointer: {#0}", (int) address);
```

Tipos de argumento soportados: `bool`, `std::string`, `char`, `char*`, `byte`, `short`, `int`, `unsigned int`, `long`, `unsigned long`, `long long`, `unsigned long long`, `float`, `double`, `long double`.

### Eliminación de niveles de log en tiempo de compilación

Definí uno de los siguientes antes de incluir `OutputLogger.h` para eliminar por completo las llamadas de log de menor prioridad (útil para builds de release):

- `ODIS_V` — elimina `OLOG_V` / `OLOG_VF`
- `ODIS_VLW` — elimina `OLOG_V`, `OLOG_L`, `OLOG_W` y sus variantes `*_F`

## Compilación

COLogger apunta a **Windows** y se compila con **CMake** (3.20+, C++20).

```
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

Alternativamente, abrí la carpeta del repositorio directamente en Visual Studio 2022 o Visual Studio 2026. Cualquier otro generador de CMake (Ninja, etc.) también funciona, siempre que el compilador sea compatible con MSVC.

La compilación genera dos targets a la vez, ambos llamados `COLogger`:

- `COLogger_static` — librería estática
- `COLogger_shared` — librería compartida (DLL) con la ABI de C habilitada automáticamente (`DLL` se define solo para este target)

Cuando se compila como proyecto de nivel superior, los binarios se generan en `bin/<Config> - <Platform>/Static/` y `bin/<Config> - <Platform>/Shared/`, con `OutputLogger.h` copiado junto a cada uno.

### Consumir desde otro proyecto CMake

```cmake
add_subdirectory(external/COLogger)
target_link_libraries(MyApp PRIVATE COLogger_static) # o COLogger_shared
```

Cuando se incluye de esta forma (es decir, sin ser el proyecto CMake de nivel superior), COLogger no redirige su salida a `bin/` ni copia los headers — simplemente compila los targets de librería para que los enlaces.

## Usarlo desde C o C#

Definir el símbolo de preprocesador `DLL` expone una ABI de C plana al final de `OutputLogger.h`, pensada para consumidores que no son C++. El target de CMake `COLogger_shared` ya lo define por vos, así que al compilarlo obtenés las funciones exportadas de inmediato:

```c
void InitLogSys(bool useConsole, bool logToFile);
void Log(int logLevel, char* message);
void ClearConsole();
```

Esto permite que un proyecto en C o C# (vía P/Invoke) controle el logger sin enlazar contra símbolos de C++ con nombres decorados (name-mangled). Tené en cuenta que la API con plantillas `OFORMAT`/`FormatMsg` no puede cruzar el límite de una DLL, así que el logging formateado no está disponible desde la ABI de C — construí el string del lado de quien llama antes de pasarlo a `Log`.

## Estructura del proyecto

```
COLogger/
├── CMakeLists.txt            # Configuración de CMake de nivel superior
└── COLogger/
    ├── CMakeLists.txt         # Targets de librería (estática + compartida)
    ├── OutputLogger.h         # API pública: clase OutputLogger y macros de logging
    ├── OutputLogger.cpp       # Implementación
    └── pch.h / pch.cpp        # Precompiled header
```

## Estado del proyecto

COLogger es un proyecto personal en evolución activa. Carencias conocidas y trabajo en curso:

- Hay un problema conocido de asignación de memoria en la generación del timestamp por mensaje, que todavía se está investigando.
- Todavía no hay una suite de tests automatizada.
- Mal manejo de tipos al formatear, termina en una Exception abrupta.
- Debería tener una representación más abstracta de un 'sink', para que sea más fácil crear otros nuevos.
- Agregar formateo para las librerías con ABI de C habilitada.

Se aceptan contribuciones, issues y sugerencias.
