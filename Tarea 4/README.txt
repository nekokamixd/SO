-José Montecinos 201804541-1
-Sofía Palet     201873570-1

Detalles de la implementación:
-La implementación se basa en semáforos binarios como candados para las secciones 
críticas, eventos como variables de condición y semáforos no binarios como contadores.

-Se implementaron mesas como mecanismos para poder dejar las bandejas de los clientes
mientras: Juan las llena con comida o los clientes comen en ellas.

-Las zonas críticas primeramente corresponden a las pilas de bandejas y mesas, pero 
además se incorporó un ordenamiento previo para el acceso a las zonas críticas.
Los clientes deben de "tomar número" antes de acceder a alguna de las pilas de bandejas,
esto para poder saber de antemano si alguna de las pilas esta vacía y no bloquear el 
acceso a ellas. Esta "toma de número" también constituye una zona critica, ya que se
levantan banderas en estas secciones cuando se cumplen ciertas condiciones(es decir,
se modifican variables).