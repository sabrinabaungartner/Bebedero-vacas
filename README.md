# Bebedero-vacas
El objetivo de este repositorio es almacenar el proyecto, como así también documentos relacionados al proyecto final de carrera Ingeniería en Computación.

## Objetivos
* Asociar dos esp32 con dos LoRa
  * esp32 con un LoRa hacen de transmisores, y el otro LoRa con el esp32 hacen de receptores.
* Sensor de temperatura en el bebedero --> para indicar cuando resulta necesario cambiar el agua.
* Sensor de nivel del agua --> para indicar cuándo se debe rellenar el mismo.
* Sensor de ph --> determina si el agua estancada comienza degradar su calidad.
* Conteo de la cantidad de días sin llenar el bebedero --> determina si el agua estancada comienza degradar su calidad.
* Tomar métricas de cantidad de agua consumida en un día (fecha y consumición).
  * Tipo de vaca: lechera, para carne. Consumirán más o menos líquido. Dependiendo del tipo de vaca, si el ganado es de carne consume 3 litros por día y por cada kilogramo de materia seca consumida, con una temperatura media anual de 5°C. Para un ganado de leche, una vaca lechera consume de 3 a 4 litros de agua por cada kilogramo de leche producida y de 3 a 4 litros de agua por cada kilogramo de materia seca consumida en su dieta.
* App para dueño del campo
  * La app estará tomando valores desde Firebase y decidirá si es necesario enviar una notificación al usuario para informarle sobre algún cambio relevante:
    * Agua en mal estado.
    * Filtrado del agua.
    * Temperatura del agua por arriba o debajo de un cierto umbral.
    * Nivel bajo de agua.
* Hay un LoRa que se va a encontrar en los bebederos. Este dispositivo debe recibir los censados cada cierta cantidad de tiempo y enviárselos al otro LoRa, el cual tomará esos valores y los guardará en la db.
