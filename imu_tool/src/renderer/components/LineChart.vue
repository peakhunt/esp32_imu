<script>
  import { Line, mixins } from 'vue-chartjs'

  export default {
    extends: Line,
    mixins: [mixins.reactiveProp],
    props: ['chartData', 'options'],
    mounted () {
      this.renderChart(this.chartData, this.options)
    },
    methods: {
      refresh: function () {
        this.$data._chart.update()
      }
    }
  }
</script>
