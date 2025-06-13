<?php
$koneksi = mysqli_connect("localhost", "root","","db_smartlight1");

// $ldr =$_GET['ldr'];
// $distanc=$_GET['distance'];
// $suhu =$_GET['suhu'];
// $kelembapan =$_GET['kelembapan'];
// $led =$_GET['led'];

$ldr        = $_GET['ldr'];
$distance   = $_GET['distance'];
$suhu       = $_GET['suhu'];
$kelembapan = $_GET['kelembapan'];

// mysqli_query($koneksi, "ALTER TABLE tb_sensor AUTO_INCREMENT = 1");
// $simpan = mysqli_query($koneksi, "INSERT INTO tb_sensor (ldr_value,distance,suhu,kelembaban) VALUES ('$suhu', '$kelembapan', '$ldr', '$distanc')");
// if ($simpan) {
//     echo "Data berhasil disimpan";
// } else {
//     echo "Gagal menyimpan data: " . mysqli_error($koneksi);
// }

$query = "INSERT INTO tb_sensor (ldr_value, distance, suhu, kelembaban)
          VALUES ('$ldr', '$distance', '$suhu', '$kelembapan')";

$simpan = mysqli_query($koneksi, $query);

if ($simpan) {
    echo "✅ Data berhasil disimpan";
} else {
    echo "❌ Gagal menyimpan data: " . mysqli_error($koneksi);
}

?>  