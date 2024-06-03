package com.example.proyecto_final_bebedero
import com.github.mikephil.charting.data.Entry

class MyLevelHistoryActivity : BaseHistoryActivity() {
    private val firebaseDatabaseHandler: FirebaseDatabaseInterface = FirebaseDatabaseHandler()

    override fun getChartDescription(): String = ""

    override fun getDataLabel(): String = "Niveles por fecha"

    override fun getAndProcessData() {
        firebaseDatabaseHandler.getWaterLevelsSortedByDate { levelMap ->
            val entries: ArrayList<Entry> = ArrayList()
            val dates: ArrayList<String> = ArrayList()

            var index = 0f
            for ((date, temperatures) in levelMap) {
                val averageTemperature = temperatures.average().toFloat()
                entries.add(Entry(index, averageTemperature))
                dates.add(date)
                index += 1f
            }

            actualizarChart(entries, dates)
        }
    }
}