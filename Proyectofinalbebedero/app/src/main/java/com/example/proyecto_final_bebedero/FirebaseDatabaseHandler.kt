package com.example.proyecto_final_bebedero

import android.util.Log
import com.google.firebase.database.DataSnapshot
import com.google.firebase.database.DatabaseError
import com.google.firebase.database.DatabaseReference
import com.google.firebase.database.FirebaseDatabase
import com.google.firebase.database.ValueEventListener
import java.text.SimpleDateFormat
import java.util.Locale

class FirebaseDatabaseHandler : FirebaseDatabaseInterface {
    private val mDatabase: DatabaseReference = FirebaseDatabase.getInstance().reference // Get database reference

    override fun getMaxWaterTemperature(listener: (Double) -> Unit) {
        val maxWaterTemperatureRef = mDatabase.child("UsersData").child("zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2").child("cattle_waterer_1").child("parametros").child("max_water_temperature") // Get max temperature reference

        // Listen to changes in max water temperature value
        maxWaterTemperatureRef.addValueEventListener(object : ValueEventListener {
            override fun onDataChange(dataSnapshot: DataSnapshot) { // New change in value
                val maxWaterTemperature = dataSnapshot.getValue(Double::class.java) ?: 0.0 // Get new max temperature value
                listener(maxWaterTemperature) // Callback listener with new obtained value
            }

            override fun onCancelled(databaseError: DatabaseError) {
                Log.d("FirebaseDatabaseHandler", "Error in getMaxWaterTemperature function")
            }
        })
    }

    /*override fun getWaterTemperatures(listener: (List<Double>) -> Unit) {
        val backupsRef = mDatabase.child("UsersData").child("zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2").child("cattle_waterer_1").child("backup_data")

        // Listener to get backups and last_filling_date
        backupsRef.addValueEventListener(object : ValueEventListener {
            override fun onDataChange(dataSnapshot: DataSnapshot) {
                val temperatures = mutableListOf<Double>()
                val lastFillingDateString = dataSnapshot.child("last_filling_date").getValue(String::class.java)
                if (lastFillingDateString != null) {
                    val lastFillingDate = SimpleDateFormat("yyyyMMdd_HHmmss", Locale.getDefault()).parse(lastFillingDateString)

                    val sortedBackups = dataSnapshot.children.mapNotNull { backupSnapshot ->
                        val backupDate = backupSnapshot.child("date").getValue(String::class.java)
                        backupDate?.let {
                            Pair(SimpleDateFormat("dd/MM/yyyy HH:mm:ss", Locale.getDefault()).parse(backupDate), backupSnapshot)
                        }
                    }.sortedByDescending { it.first }

                    var count = 0
                    for ((backupDateTime, backupSnapshot) in sortedBackups) {
                        if (backupDateTime != null) {
                            if (backupDateTime.after(lastFillingDate)) {
                                val temperature = backupSnapshot.child("water_temperature").getValue(Double::class.java)
                                if (temperature != null) {
                                    temperatures.add(temperature)
                                    count++
                                    if (count >= 10) {
                                        break
                                    }
                                }
                            }
                        }
                    }
                }

                if (temperatures.size == 10)
                    listener(temperatures) // Callback with filtered temperatures
                else listener(emptyList())
            }

            override fun onCancelled(databaseError: DatabaseError) {
                Log.d("FirebaseDatabaseHandler", "Error en getWaterTemperatures")
            }
        })
    }*/

    override fun getWaterTemperatures(listener: (List<Double>) -> Unit) {
        val backupsRef = mDatabase.child("UsersData").child("zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2").child("cattle_waterer_1").child("backup_data")

        backupsRef.addValueEventListener(object : ValueEventListener {
            override fun onDataChange(dataSnapshot: DataSnapshot) {
                val temperatures = mutableListOf<Double>()

                var latestBackup: DataSnapshot? = null
                var latestTimestamp: String? = null

                // Iterar sobre los nodos para encontrar el más reciente
                for (backupSnapshot in dataSnapshot.children) {
                    val timestamp = backupSnapshot.key
                    if (latestTimestamp == null || (timestamp?.compareTo(latestTimestamp)
                            ?: 0) > 0
                    ) {
                        latestBackup = backupSnapshot
                        latestTimestamp = timestamp
                    }
                }

                latestBackup?.let { lastBackupSnapshot ->
                    // Obtener los últimos 10 backups dentro del directorio más reciente
                    val sortedBackups = lastBackupSnapshot.children.sortedByDescending { it.key }.take(10)

                    // Iterar sobre los últimos 10 backups
                    for (backupSnapshot in sortedBackups) {
                        // Obtener el valor de water_temperature de cada backup
                        val temperature = backupSnapshot.child("water_temperature").getValue(Double::class.java)
                        temperature?.let { temperatures.add(it) }
                        Log.d("FirebaseDatabaseHandler", "temperature $temperature")
                    }
                }

                // Si hay menos de 10 elementos, devolver una lista vacía
                if (temperatures.size < 10) {
                    listener(emptyList())
                } else {
                    // Llamar al listener con la lista de temperaturas de agua obtenida
                    listener(temperatures)
                }
            }

            override fun onCancelled(databaseError: DatabaseError) {
                Log.d("FirebaseDatabaseHandler", "Error en getWaterTemperatures")
                listener(emptyList())
            }
        })
    }


    /*override fun getWaterLevels(listener: (List<Double>) -> Unit) {
        val backupsRef = mDatabase.child("UsersData").child("zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2").child("cattle_waterer_1").child("backup_data")

        backupsRef.addValueEventListener(object : ValueEventListener {
            override fun onDataChange(dataSnapshot: DataSnapshot) {
                val levels = mutableListOf<Double>()
                val lastFillingDateString = dataSnapshot.child("last_filling_date").getValue(String::class.java)
                if (lastFillingDateString != null) {
                    val lastFillingDate = SimpleDateFormat("yyyyMMdd_HHmmss", Locale.getDefault()).parse(lastFillingDateString)

                    val sortedBackups = dataSnapshot.children.mapNotNull { backupSnapshot ->
                        val backupDate = backupSnapshot.child("date").getValue(String::class.java)
                        backupDate?.let {
                            Pair(SimpleDateFormat("dd/MM/yyyy HH:mm:ss", Locale.getDefault()).parse(backupDate), backupSnapshot)
                        }
                    }.sortedByDescending { it.first }

                    var count = 0
                    for ((backupDateTime, backupSnapshot) in sortedBackups) {
                        if (backupDateTime != null) {
                            if (backupDateTime.after(lastFillingDate)) {
                                val level = backupSnapshot.child("water_level").getValue(Double::class.java)
                                if (level != null) {
                                    levels.add(level)
                                    count++
                                    if (count >= 10) {
                                        break
                                    }
                                }
                            }
                        }
                    }
                }

                if (levels.size == 10)
                    listener(levels) // Callback with filtered levels
                else listener(emptyList())
            }

            override fun onCancelled(databaseError: DatabaseError) {
                Log.d("FirebaseDatabaseHandler", "Error en getWaterLevels")
            }
        })
    }*/

    override fun getWaterLevels(listener: (List<Double>) -> Unit) {
        val backupsRef = mDatabase.child("UsersData").child("zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2").child("cattle_waterer_1").child("backup_data")

        backupsRef.addValueEventListener(object : ValueEventListener {
            override fun onDataChange(dataSnapshot: DataSnapshot) {
                val levels = mutableListOf<Double>()

                var latestBackup: DataSnapshot? = null
                var latestTimestamp: String? = null

                // Iterar sobre los nodos para encontrar el más reciente
                for (backupSnapshot in dataSnapshot.children) {
                    val timestamp = backupSnapshot.key
                    if (latestTimestamp == null || (timestamp?.compareTo(latestTimestamp)
                            ?: 0) > 0
                    ) {
                        latestBackup = backupSnapshot
                        latestTimestamp = timestamp
                    }
                }

                Log.d("FirebaseDatabaseHandler", "latestBackup $latestBackup")
                latestBackup?.let { lastBackupSnapshot ->
                    // Obtener los últimos 10 backups dentro del directorio más reciente
                    val sortedBackups = lastBackupSnapshot.children.sortedByDescending { it.key }.take(10)

                    // Iterar sobre los últimos 10 backups
                    for (backupSnapshot in sortedBackups) {
                        // Obtener el valor de water_temperature de cada backup
                        val level = backupSnapshot.child("water_level").getValue(Double::class.java)
                        level?.let { levels.add(it) }
                        Log.d("FirebaseDatabaseHandler", "level $level")
                    }
                }

                // Si hay menos de 10 elementos, devolver una lista vacía
                if (levels.size < 10) {
                    listener(emptyList())
                } else {
                    // Llamar al listener con la lista de niveles de agua obtenida
                    listener(levels)
                }
            }

            override fun onCancelled(databaseError: DatabaseError) {
                Log.d("FirebaseDatabaseHandler", "Error en getWaterLevels")
                listener(emptyList())
            }
        })
    }


    override fun getMinWaterLevel(listener: (Int) -> Unit) {
        val minWaterLevelRef = mDatabase.child("UsersData").child("zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2").child("cattle_waterer_1").child("parametros").child("min_water_level") //Ref maxWaterLevel

        // listener max water level
        minWaterLevelRef.addValueEventListener(object : ValueEventListener {
            override fun onDataChange(dataSnapshot: DataSnapshot) {
                val minWaterLevel = dataSnapshot.getValue(Int::class.java) ?: 0 // get min water level
                listener(minWaterLevel) // call callback with value obtained
            }

            override fun onCancelled(databaseError: DatabaseError) {
                Log.d("FirebaseDatabaseHandler", "Error in getMinWaterLevel function")
            }
        })
    }

    override fun getWaterQuality(listener: (Int) -> Unit) {
        val currentWaterQuality = mDatabase.child("UsersData").child("zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2").child("cattle_waterer_1").child("current_data").child("days_without_filling") //Ref days_without_filling

        // Listener when water quality value changes
        currentWaterQuality.addValueEventListener(object : ValueEventListener {
            override fun onDataChange(dataSnapshot: DataSnapshot) {
                val currentWaterQualityValue = dataSnapshot.getValue(Int::class.java) ?: 99 // Get current water quality value
                listener(currentWaterQualityValue) // Callback with obtained value
            }

            override fun onCancelled(databaseError: DatabaseError) {
                Log.d("FirebaseDatabaseHandler", "Error in getWaterQuality function")
            }
        })
    }

    override fun getWaterMaxDaysWithoutFill(listener: (Int) -> Unit) {
        val currentMaxDaysWithoutFill = mDatabase.child("UsersData").child("zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2").child("cattle_waterer_1").child("parametros").child("max_days_without_filling") //Ref max_days_without_filling

        // Agregar el listener para la temperatura máxima
        currentMaxDaysWithoutFill.addValueEventListener(object : ValueEventListener {
            override fun onDataChange(dataSnapshot: DataSnapshot) {
                val currentMaxDaysWithoutFillValue = dataSnapshot.getValue(Int::class.java) ?: 99 // Get current water max quality value
                listener(currentMaxDaysWithoutFillValue) // Callback with obtained value
            }

            override fun onCancelled(databaseError: DatabaseError) {
                Log.d("FirebaseDatabaseHandler", "Error in getWaterMaxDaysWithoutFill function")
            }
        })
    }

    override fun getWaterMedDaysWithoutFill(listener: (Int) -> Unit) {
        val currentMedDaysWithoutFill = mDatabase.child("UsersData").child("zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2").child("cattle_waterer_1").child("parametros").child("med_days_without_filling") //Ref dias_sin_rellenar

        // Agregar el listener para la temperatura máxima
        currentMedDaysWithoutFill.addValueEventListener(object : ValueEventListener {
            override fun onDataChange(dataSnapshot: DataSnapshot) {
                val currentMedDaysWithoutFillValue = dataSnapshot.getValue(Int::class.java) ?: 99 // Get current water ed quality value
                listener(currentMedDaysWithoutFillValue) // Callback with obtained value
            }

            override fun onCancelled(databaseError: DatabaseError) {
                Log.d("FirebaseDatabaseHandler", "Error in getWaterMedDaysWithoutFill function")
            }
        })
    }

    override fun setMaxWaterTemperature(value: Double) {
        val maxWaterTemperatureRef = mDatabase.child("UsersData").child("zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2").child("cattle_waterer_1").child("parametros").child("max_water_temperature") // Reference to max temperature

        // Set the new max water temperature value in Firebase
        maxWaterTemperatureRef.setValue(value)
            .addOnSuccessListener {
                Log.d("FirebaseDatabaseHandler", "Max water temperature updated successfully to $value")
            }
            .addOnFailureListener { e ->
                Log.d("FirebaseDatabaseHandler", "Error updating max water temperature", e)
            }
    }

    override fun setWaterMaxDaysWithoutFill(value: Int) {
        val maxDaysWithoutFill = mDatabase.child("UsersData").child("zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2").child("cattle_waterer_1").child("parametros").child("max_days_without_filling") // Reference to max_days_without_filling

        // Set the new max days without fill value in Firebase
        maxDaysWithoutFill.setValue(value)
            .addOnSuccessListener {
                Log.d("FirebaseDatabaseHandler", "Max days without fill updated successfully to $value")
            }
            .addOnFailureListener { e ->
                Log.d("FirebaseDatabaseHandler", "Error updating max days without fill", e)
            }
    }

    override fun setWaterMedDaysWithoutFill(value: Int) {
        val medDaysWithoutFill = mDatabase.child("UsersData").child("zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2").child("cattle_waterer_1").child("parametros").child("med_days_without_filling") // Reference to med_days_without_filling

        // Set the new max days without fill value in Firebase
        medDaysWithoutFill.setValue(value)
            .addOnSuccessListener {
                Log.d("FirebaseDatabaseHandler", "Med days without fill updated successfully to $value")
            }
            .addOnFailureListener { e ->
                Log.d("FirebaseDatabaseHandler", "Error updating med days without fill", e)
            }
    }

    override fun setFillWaterer(value: Int) {
        val fillWatererReference = mDatabase.child("UsersData").child("zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2").child("cattle_waterer_1").child("fill_waterer") // Reference to fill_waterer

        fillWatererReference.setValue(value)
            .addOnSuccessListener {
                Log.d("FirebaseDatabaseHandler", "value of fill_waterer updated successfully to $value")
            }
            .addOnFailureListener { e ->
                Log.d("FirebaseDatabaseHandler", "Error updating value of fill_waterer", e)
            }
    }

    override fun getFillWaterer(listener: (Int) -> Unit) {
        val fillWatererRef = mDatabase.child("UsersData").child("zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2").child("cattle_waterer_1").child("fill_waterer") // Reference to fill_waterer

        // Listener to detect changes on fill_waterer value
        fillWatererRef.addValueEventListener(object : ValueEventListener {
            override fun onDataChange(snapshot: DataSnapshot) {
                val fillWatererValue = snapshot.getValue(Int::class.java)
                if (fillWatererValue != null) { // fill_waterer is not empty
                    listener(fillWatererValue)
                } else {
                    listener(0) // The default value is zero
                }
            }

            override fun onCancelled(error: DatabaseError) {
                // Handle operation cancellation
                Log.d("FirebaseDatabaseHandler", "Error on lecture fill_waterer")
            }
        })
    }

    /*override fun getWaterTemperaturesSortedByDate(listener: (Map<String, List<Double>>) -> Unit) {
        val backupsRef = mDatabase.child("UsersData").child("zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2").child("cattle_waterer_1").child("backup_data")

        backupsRef.addValueEventListener(object : ValueEventListener {
            override fun onDataChange(dataSnapshot: DataSnapshot) {
                val temperatureMap = mutableMapOf<String, MutableList<Double>>()

                for (backupSnapshot in dataSnapshot.children) {
                    val backupDate = backupSnapshot.child("date").getValue(String::class.java)
                    val temperature = backupSnapshot.child("water_temperature").getValue(Double::class.java)

                    if (backupDate != null && temperature != null) {
                        val dateWithoutTime = backupDate.substring(0, 10) // Get date without time

                        if (!temperatureMap.containsKey(dateWithoutTime)) {
                            temperatureMap[dateWithoutTime] = mutableListOf()
                        }
                        temperatureMap[dateWithoutTime]?.add(temperature)
                    }
                }

                listener(temperatureMap)
            }

            override fun onCancelled(databaseError: DatabaseError) {
                Log.d("FirebaseDatabaseHandler", "Error in getWaterTemperaturesSortedByDate function")
                listener(emptyMap()) // Return an empty map in case of error
            }
        })
    }

    override fun getWaterLevelsSortedByDate(listener: (Map<String, List<Double>>) -> Unit) {
        val backupsRef = mDatabase.child("UsersData").child("zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2").child("cattle_waterer_1").child("backup_data")

        backupsRef.addValueEventListener(object : ValueEventListener {
            override fun onDataChange(dataSnapshot: DataSnapshot) {
                val levelMap = mutableMapOf<String, MutableList<Double>>()

                for (backupSnapshot in dataSnapshot.children) {
                    val backupDate = backupSnapshot.child("date").getValue(String::class.java)
                    val level = backupSnapshot.child("water_level").getValue(Double::class.java)

                    if (backupDate != null && level != null) {
                        val dateWithoutTime = backupDate.substring(0, 10) // Get date without time

                        if (!levelMap.containsKey(dateWithoutTime)) {
                            levelMap[dateWithoutTime] = mutableListOf()
                        }
                        levelMap[dateWithoutTime]?.add(level)
                    }
                }

                listener(levelMap)
            }

            override fun onCancelled(databaseError: DatabaseError) {
                Log.d("FirebaseDatabaseHandler", "Error in getWaterLevelsSortedByDate function")
                listener(emptyMap()) // Return an empty map in case of error
            }
        })
    }*/

    override fun getWaterTemperaturesSortedByDate(listener: (Map<String, List<Double>>) -> Unit) {
        val backupsRef = mDatabase.child("UsersData").child("zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2").child("cattle_waterer_1").child("backup_data")

        backupsRef.addValueEventListener(object : ValueEventListener {
            override fun onDataChange(dataSnapshot: DataSnapshot) {
                val temperatureMap = mutableMapOf<String, MutableList<Double>>()

                for (backupDateSnapshot in dataSnapshot.children) {
                    for (backupSnapshot in backupDateSnapshot.children) {
                        val backupDate = backupSnapshot.child("date").getValue(String::class.java)
                        val temperature = backupSnapshot.child("water_temperature").getValue(Double::class.java)

                        if (backupDate != null && temperature != null) {
                            val dateWithoutTime = backupDate.substring(0, 10) // Get date without time

                            if (!temperatureMap.containsKey(dateWithoutTime)) {
                                temperatureMap[dateWithoutTime] = mutableListOf()
                            }
                            temperatureMap[dateWithoutTime]?.add(temperature)
                        }
                    }
                }

                listener(temperatureMap)
            }

            override fun onCancelled(databaseError: DatabaseError) {
                Log.d("FirebaseDatabaseHandler", "Error in getWaterTemperaturesSortedByDate function")
                listener(emptyMap()) // Return an empty map in case of error
            }
        })
    }

    override fun getWaterLevelsSortedByDate(listener: (Map<String, List<Double>>) -> Unit) {
        val backupsRef = mDatabase.child("UsersData").child("zmEF5GNXqOTqIzXlmnjdJ4EQ4NK2").child("cattle_waterer_1").child("backup_data")

        backupsRef.addValueEventListener(object : ValueEventListener {
            override fun onDataChange(dataSnapshot: DataSnapshot) {
                val levelMap = mutableMapOf<String, MutableList<Double>>()

                for (backupDateSnapshot in dataSnapshot.children) {
                    for (backupSnapshot in backupDateSnapshot.children) {
                        val backupDate = backupSnapshot.child("date").getValue(String::class.java)
                        val level = backupSnapshot.child("water_level").getValue(Double::class.java)

                        if (backupDate != null && level != null) {
                            val dateWithoutTime = backupDate.substring(0, 10) // Get date without time

                            if (!levelMap.containsKey(dateWithoutTime)) {
                                levelMap[dateWithoutTime] = mutableListOf()
                            }
                            levelMap[dateWithoutTime]?.add(level)
                        }
                    }
                }

                listener(levelMap)
            }

            override fun onCancelled(databaseError: DatabaseError) {
                Log.d("FirebaseDatabaseHandler", "Error in getWaterLevelsSortedByDate function")
                listener(emptyMap()) // Return an empty map in case of error
            }
        })
    }

}

