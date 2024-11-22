# instrucciones claras para compilar, ejecutar y utilizar la shell y el programa de monitoreo.
aca debo poner las instruccione s de uso de la shell


# compilacion:

cmake .. -DCMAKE_TOOLCHAIN_FILE=./build/Release/generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release

make -j$(nproc)



# INSTRUCCIONES DE COMANDOS

## cd :
    vuelve al directorio home principal

## cd <directorio>:
    Cambia el directorio actual a <directorio>

## cd - : retorna al último directorio visitado. me vuelve al diretorio que estaba antes

## cd..  :

## cd /directorionuevo:
    cambia a un directorio en especificoc

## pwd:
    imprime directorio actual

## clr :
    limpia la pantalla. comando realizado por mi

## clear :
    limpia la pantalla . comando ya definido externo


## echo comentario:
    imprime un comentario

## echo $VARIABLEDEENTORNO:
     imprime una variable de entorno

## start_monitor:
    inicializa las metricas

## status_monitor:
        muestra las metricas por consola

## stop_monitor:
    detiene el proceso de metricas

## quit :
    sale de la consola

## exit:
    sale de la consola
