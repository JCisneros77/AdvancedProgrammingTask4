# PATarea4
Programación Avanzada - Tarea 4: Concurrencia

# 1a)
Los problemas que se dan en estos sistemas es en la memoria que comparten los threads, ya que puede ocurrir que uno escriba mientras otro lea, y así ocurriría una corrupción de la información. Generalmente este es el problema en estos sistemas.

	Otro problema es que se pueden ocasionar 'locks' en los que todo el programa se bloquea y no puede continuar así que hay que tener mucho cuidado al momento de manejar los semáforos para que esto no suceda.

# 1b)	
	Lo voy a solucionar utilizando semáforos binarios, o mutex, ya que evitan el problema de una posible corrupción de la información. También solucionaré el problema de los posibles bloqueos realizando un buen manejo de los mutex. 
