package com.example.proyecto_final_bebedero

import android.util.Log
import com.google.firebase.database.DataSnapshot
import com.google.firebase.database.DatabaseError
import com.google.firebase.database.DatabaseReference
import com.google.firebase.database.FirebaseDatabase
import com.google.firebase.database.ValueEventListener

/**class FirebaseDatabaseHandler : FirebaseDatabaseInterface {
    private val mDatabase: DatabaseReference = FirebaseDatabase.getInstance().reference //get database reference

    override fun getMaxWaterTemperature(listener: (Double) -> Unit) {
        val maxWaterTemperatureRef = mDatabase.child("parametros").child("temperatura_maxima") //get max water temperature reference

        //Listen for changes on max water temperature parameter
        maxWaterTemperatureRef.addValueEventListener(object : ValueEventListener {
            override fun onDataChange(dataSnapshot: DataSnapshot) { //called when changes exists
                val maxWaterTemperatureRefChange = dataSnapshot.getValue(Double::class.java) ?: 0.0 //get max water temperature value
                listener(maxWaterTemperatureRefChange)
            }

            override fun onCancelled(databaseError: DatabaseError) {
                val errorMessage = "Error reading from Firebase: ${databaseError.message}"
                Log.e("FirebaseDatabaseHandler", errorMessage)
                // También puedes mostrar un mensaje de error en la interfaz de usuario si lo deseas
                // onFailureListener(errorMessage)
            }
        })
    }

    override fun calculateAverageWaterTemperature(onSuccessListener: (Double) -> Unit, onFailureListener: (String) -> Unit) {
        val backupsRef = mDatabase.child("datos_backup")

        backupsRef.addValueEventListener(object : ValueEventListener {
            override fun onDataChange(dataSnapshot: DataSnapshot) {
                var totalTemperature = 0.0
                var numBackups = 0

                // Iterar sobre cada backup
                for (backupSnapshot in dataSnapshot.children) {
                    val temperature = backupSnapshot.child("temperatura_agua").getValue(Double::class.java)
                    // Verificar si la temperatura está disponible en el backup
                    if (temperature != null) {
                        totalTemperature += temperature
                        numBackups++
                    }
                }

                // Calcular el promedio de la temperatura del agua
                val averageTemperature = if (numBackups > 0) totalTemperature / numBackups else 0.0
                onSuccessListener(averageTemperature)
            }

            override fun onCancelled(databaseError: DatabaseError) {
                val errorMessage = "Error reading from Firebase: ${databaseError.message}"
                Log.e("FirebaseDatabaseHandler", errorMessage)
                // También puedes mostrar un mensaje de error en la interfaz de usuario si lo deseas
                // onFailureListener(errorMessage)

                //onFailureListener(databaseError.message)
            }
        })
    }
}**/

class FirebaseDatabaseHandler : FirebaseDatabaseInterface {
    private val mDatabase: DatabaseReference = FirebaseDatabase.getInstance().reference // Obtener la referencia de la base de datos

    override fun getMaxWaterTemperature(listener: (Double) -> Unit) {
        val maxWaterTemperatureRef = mDatabase.child("parametros").child("temperatura_maxima") // Obtener la referencia de la temperatura máxima

        // Agregar el listener para la temperatura máxima
        maxWaterTemperatureRef.addValueEventListener(object : ValueEventListener {
            override fun onDataChange(dataSnapshot: DataSnapshot) {
                val maxWaterTemperature = dataSnapshot.getValue(Double::class.java) ?: 0.0 // Obtener el valor de la temperatura máxima
                listener(maxWaterTemperature) // Llamar al callback con el valor obtenido
            }

            override fun onCancelled(databaseError: DatabaseError) {
                // Manejar el error, si es necesario
            }
        })
    }

    override fun getWaterTemperatures(listener: (List<Double>) -> Unit) {
        val backupsRef = mDatabase.child("datos_backup") // Obtener la referencia de los datos de backup

        // Agregar el listener para los datos de backup
        backupsRef.addValueEventListener(object : ValueEventListener {
            override fun onDataChange(dataSnapshot: DataSnapshot) {
                val temperatures = mutableListOf<Double>()

                // Iterar sobre cada backup
                for (backupSnapshot in dataSnapshot.children) {
                    val temperature = backupSnapshot.child("temperatura_agua").getValue(Double::class.java)
                    // Verificar si la temperatura está disponible en el backup
                    if (temperature != null) {
                        temperatures.add(temperature)
                    }
                }

                listener(temperatures) // Llamar al callback con la lista de temperaturas obtenidas
            }

            override fun onCancelled(databaseError: DatabaseError) {
                // Manejar el error, si es necesario
            }
        })
    }

    override fun getWaterLevels(listener: (List<Double>) -> Unit) {
        val backupsRef = mDatabase.child("datos_backup") // get reference to backup values

        // Agregar el listener para los datos de backup
        backupsRef.addValueEventListener(object : ValueEventListener {
            override fun onDataChange(dataSnapshot: DataSnapshot) {
                val level_values = mutableListOf<Double>()

                // Iterar sobre cada backup
                for (backupSnapshot in dataSnapshot.children) {
                    val level = backupSnapshot.child("temperatura_agua").getValue(Double::class.java)
                    // Verificar si la temperatura está disponible en el backup
                    if (level != null) {
                        level_values.add(level)
                    }
                }

                listener(level_values) // Llamar al callback con la lista de temperaturas obtenidas
            }

            override fun onCancelled(databaseError: DatabaseError) {
                // Manejar el error, si es necesario
            }
        })
    }

    override fun getMaxWaterLevel(listener: (Double) -> Unit) {
        val maxWaterLevelRef = mDatabase.child("parametros").child("nivel_agua") //Ref maxWaterLevel

        // listener max water level
        maxWaterLevelRef.addValueEventListener(object : ValueEventListener {
            override fun onDataChange(dataSnapshot: DataSnapshot) {
                val maxWaterLevel = dataSnapshot.getValue(Double::class.java) ?: 0.0 // get max water level
                listener(maxWaterLevel) // call callback with value obtained
            }

            override fun onCancelled(databaseError: DatabaseError) {
                // Manejar el error, si es necesario
            }
        })
    }
}
