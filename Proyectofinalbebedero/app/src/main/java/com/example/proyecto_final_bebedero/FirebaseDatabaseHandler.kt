package com.example.proyecto_final_bebedero
import android.util.Log
import com.google.firebase.database.DataSnapshot
import com.google.firebase.database.DatabaseError
import com.google.firebase.database.DatabaseReference
import com.google.firebase.database.FirebaseDatabase
import com.google.firebase.database.ValueEventListener

class FirebaseDatabaseHandler : FirebaseDatabaseInterface {
    private val mDatabase: DatabaseReference = FirebaseDatabase.getInstance().reference

    override fun addTemperatureMaxListener(listener: (Double) -> Unit) {
        val temperaturaMaximaRef = mDatabase.child("parametros").child("temperatura_maxima")

        temperaturaMaximaRef.addValueEventListener(object : ValueEventListener {
            override fun onDataChange(dataSnapshot: DataSnapshot) {
                val temperaturaMaxima = dataSnapshot.getValue(Double::class.java) ?: 0.0
                listener(temperaturaMaxima)
            }

            override fun onCancelled(databaseError: DatabaseError) {
                // Manejar el error aquí si es necesario
            }
        })
    }

    override fun calcularPromedioTemperaturaAgua(onSuccessListener: (Double) -> Unit, onFailureListener: (String) -> Unit) {
        val backupsRef = mDatabase.child("datos_backup")

        backupsRef.addValueEventListener(object : ValueEventListener {
            override fun onDataChange(dataSnapshot: DataSnapshot) {
                var totalTemperatura = 0.0
                var numBackups = 0

                // Iterar sobre cada backup
                for (backupSnapshot in dataSnapshot.children) {
                    val temperatura = backupSnapshot.child("temperatura_agua").getValue(Double::class.java)
                    // Verificar si la temperatura está disponible en el backup
                    if (temperatura != null) {
                        totalTemperatura += temperatura
                        numBackups++
                    }
                }

                // Calcular el promedio de la temperatura del agua
                val promedioTemperatura = if (numBackups > 0) totalTemperatura / numBackups else 0.0
                onSuccessListener(promedioTemperatura)
            }

            override fun onCancelled(databaseError: DatabaseError) {
                onFailureListener(databaseError.message)
            }
        })
    }
}
