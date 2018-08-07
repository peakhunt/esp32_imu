<template>
  <div :width="width" :height="height" ref="threeCanvas" class="absolute_container">
  </div>
</template>

<script>
  const THREE = require('three')
  const OrbitControls = require('three-orbit-controls')(THREE)

  const MAX_POINTS = 10000

  export default {
    name: 'SphereFittingView',
    props: ['width', 'height'],
    methods: {
      do_render () {
        this.renderer.render(this.scene, this.camera)
        this.reqAnimID = requestAnimationFrame(this.do_render)
      },
      push_point (p) {
        if (this.pointCount >= MAX_POINTS) {
          return
        }

        var positions = this.points.geometry.attributes.position.array

        positions[this.pointCount * 3 + 0] = p.x
        positions[this.pointCount * 3 + 1] = p.y
        positions[this.pointCount * 3 + 2] = p.z

        this.pointCount++
        this.points.geometry.setDrawRange(0, this.pointCount)
        this.points.geometry.attributes.position.needsUpdate = true
      },
      clear_points () {
        this.pointCount = 0
        this.points.geometry.setDrawRange(0, 0)
      }
    },
    mounted () {
      var w = this.$refs.threeCanvas.offsetWidth
      var h = this.$refs.threeCanvas.offsetHeight

      console.log('w: ' + w + ',' + 'h: ' + h)

      this.camera = new THREE.PerspectiveCamera(75, this.width / this.height, 0.1, 1000)
      this.camera.position.set(0, 0, 10)

      this.renderer = new THREE.WebGLRenderer({
        alpha: false
      })
      this.renderer.setSize(this.width, this.height)
      this.$refs.threeCanvas.appendChild(this.renderer.domElement)

      this.scene = new THREE.Scene()

      this.scene.add(new THREE.AmbientLight(0x7f0000))

      var origin = new THREE.Vector3(0, 0, 0)

      var arrowXDir = new THREE.Vector3(0, 0, -1)
      var arrowYDir = new THREE.Vector3(1, 0, 0)
      var arrowZDir = new THREE.Vector3(0, 1, 0)

      this.arrowX = new THREE.ArrowHelper(arrowXDir, origin, 5, 0xff0000)
      this.arrowY = new THREE.ArrowHelper(arrowYDir, origin, 3, 0x00ff00)
      this.arrowZ = new THREE.ArrowHelper(arrowZDir, origin, 2.5, 0x0000ff)

      this.scene.add(this.arrowX)
      this.scene.add(this.arrowY)
      this.scene.add(this.arrowZ)

      var dotGeometry = new THREE.BufferGeometry()
      var positions = new Float32Array(MAX_POINTS * 3)
      dotGeometry.addAttribute('position', new THREE.BufferAttribute(positions, 3))
      dotGeometry.setDrawRange(0, 0)

      var dotMaterial = new THREE.PointsMaterial({ color: 0xffffff, size: 0.1, sizeAttenuation: true })

      this.points = new THREE.Points(dotGeometry, dotMaterial)
      this.scene.add(this.points)

      this.controls = new OrbitControls(this.camera, this.renderer.domElement)

      this.do_render()
    },
    beforeDestroy () {
      if (this.reqAnimID != null) {
        cancelAnimationFrame(this.reqAnimID)
        this.reqAnimID = null
      }
    },
    data () {
      return {
        camera: null,
        renderer: null,
        reqAnimID: null,
        scene: null,
        controls: null,
        arrowX: null,
        arrowY: null,
        arrowZ: null,
        points: null,
        pointCount: 0
      }
    }
  }
</script>

<style scoped>
</style>
