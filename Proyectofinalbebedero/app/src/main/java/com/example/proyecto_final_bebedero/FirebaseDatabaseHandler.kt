package com.example.proyecto_final_bebedero

import android.util.Log
import com.google.firebase.database.DataSnapshot
import com.google.firebase.database.DatabaseError
import com.google.firebase.database.DatabaseReference
import com.google.firebase.database.FirebaseDatabase
import com.google.firebase.database.ValueEventListener

class FirebaseDatabaseHandler : FirebaseDatabaseInterface {
    private val mDatabase: DatabaseReference = FirebaseDatabase.getInstance().reference // Get database reference

    override fun getMaxWaterTemperature(listener: (Double) -> Unit) {
        val maxWaterTemperatureRef = mDatabase.child("parametros").child("temperatura_maxima") // Get max temperature reference

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

    override fun getWaterTemperatures(listener: (List<Double>) -> Unit) {
        val backupsRef = mDatabase.child("datos_backup") // Get reference of backup water temperature values

        backupsRef.addValueEventListener(object : ValueEventListener {
            override fun onDataChange(dataSnapshot: DataSnapshot) {
                val temperatures = mutableListOf<Double>()

                for (backupSnapshot in dataSnapshot.children) { // Iterate over each backup
                    val temperature = backupSnapshot.child("temperatura_agua").getValue(Double::class.java)
                    if (temperature != null) { // Check if the temperature is available in the backup
                        temperatures.add(temperature)
                    }
                }

                listener(temperatures) // Callback with the list of temperatures obtained
            }

            override fun onCancelled(databaseError: DatabaseError) {
                Log.d("FirebaseDatabaseHandler", "Error in getWaterTemperatures function")
            }
        })
    }

    override fun getWaterLevels(listener: (List<Double>) -> Unit) {
        val backupsRef = mDatabase.child("datos_backup") // Get reference to backup values

        // Listener
        backupsRef.addValueEventListener(object : ValueEventListener {
            override fun onDataChange(dataSnapshot: DataSnapshot) {
                val levelValues = mutableListOf<Double>()

                // Iterate above backups
                for (backupSnapshot in dataSnapshot.children) {
                    val level = backupSnapshot.child("nivel_agua").getValue(Double::class.java)
                    if (level != null) {
                        levelValues.add(level)
                    }
                }

                listener(levelValues) // Callback with the list of water levels obtained
            }

            override fun onCancelled(databaseError: DatabaseError) {
                Log.d("FirebaseDatabaseHandler", "Error in getWaterLevels function")
            }
        })
    }

    override fun getMaxWaterLevel(listener: (Double) -> Unit) {
        val maxWaterLevelRef = mDatabase.child("parametros").child("nivel_agua_minimo") //Ref maxWaterLevel

        // listener max water level
        maxWaterLevelRef.addValueEventListener(object : ValueEventListener {
            override fun onDataChange(dataSnapshot: DataSnapshot) {
                val maxWaterLevel = dataSnapshot.getValue(Double::class.java) ?: 0.0 // get max water level
                listener(maxWaterLevel) // call callback with value obtained
            }

            override fun onCancelled(databaseError: DatabaseError) {
                Log.d("FirebaseDatabaseHandler", "Error in getMaxWaterLevel function")
            }
        })
    }

    override fun getWaterQuality(listener: (Int) -> Unit) {
        val currentWaterQuality = mDatabase.child("data_actual").child("dias_sin_rellenar") //Ref dias_sin_rellenar

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
        val currentMaxDaysWithoutFill = mDatabase.child("parametros").child("dias_max_sin_rellenar") //Ref dias_sin_rellenar

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
        val currentMedDaysWithoutFill = mDatabase.child("parametros").child("dias_med_sin_rellenar") //Ref dias_sin_rellenar

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
        val maxWaterTemperatureRef = mDatabase.child("parametros").child("temperatura_maxima") // Reference to max temperature

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
        val maxDaysWithoutFill = mDatabase.child("parametros").child("dias_max_sin_rellenar") // Reference to max temperature

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
        val medDaysWithoutFill = mDatabase.child("parametros").child("dias_med_sin_rellenar") // Reference to max temperature

        // Set the new max days without fill value in Firebase
        medDaysWithoutFill.setValue(value)
            .addOnSuccessListener {
                Log.d("FirebaseDatabaseHandler", "Med days without fill updated successfully to $value")
            }
            .addOnFailureListener { e ->
                Log.d("FirebaseDatabaseHandler", "Error updating med days without fill", e)
            }
    }
}
