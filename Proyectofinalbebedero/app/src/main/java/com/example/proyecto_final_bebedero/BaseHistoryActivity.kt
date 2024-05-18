package com.example.proyecto_final_bebedero

import android.graphics.Color
import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import com.github.mikephil.charting.charts.LineChart
import com.github.mikephil.charting.data.Entry
import com.github.mikephil.charting.data.LineData
import com.github.mikephil.charting.data.LineDataSet
import com.github.mikephil.charting.components.XAxis
import com.github.mikephil.charting.formatter.IndexAxisValueFormatter

abstract class BaseHistoryActivity : AppCompatActivity() {
    private lateinit var lineChart: LineChart

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_history_graph)

        lineChart = findViewById(R.id.lineChartHistory)

        getAndProcessData()
    }

    abstract fun getAndProcessData()
    abstract fun getChartDescription(): String
    abstract fun getDataLabel(): String

    private fun setupLineChart(dates: ArrayList<String>) {
        lineChart.xAxis.valueFormatter = IndexAxisValueFormatter(dates)
        lineChart.xAxis.position = XAxis.XAxisPosition.BOTTOM
        lineChart.xAxis.granularity = 1f
        lineChart.xAxis.setDrawGridLines(true)
        lineChart.xAxis.gridColor = Color.LTGRAY
        lineChart.xAxis.gridLineWidth = 1f
        lineChart.xAxis.labelRotationAngle = -45f
        lineChart.xAxis.setAvoidFirstLastClipping(true)
        lineChart.axisRight.isEnabled = false
        lineChart.description.isEnabled = true
        lineChart.description.text = getChartDescription()
        lineChart.legend.isEnabled = true

        val yAxis = lineChart.axisLeft
        yAxis.setDrawGridLines(true)
        yAxis.gridColor = Color.LTGRAY
        yAxis.gridLineWidth = 1f
    }

    protected fun actualizarChart(entries: ArrayList<Entry>, dates: ArrayList<String>) {
        val dataSet = LineDataSet(entries, getDataLabel())
        dataSet.color = Color.BLUE
        dataSet.valueTextColor = Color.GREEN
        dataSet.setDrawValues(true)

        val lineData = LineData(dataSet)

        setupLineChart(dates)

        lineChart.data = lineData
        lineChart.invalidate()
    }
}