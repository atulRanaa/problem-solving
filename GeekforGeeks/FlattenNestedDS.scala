import org.apache.spark.sql.{Dataset, SparkSession}
import org.apache.spark.sql.functions._
import org.scalatest.{FlatSpec, Matchers}


case class C(value: Int)
case class B(m: Map[Int, Seq[C]])
case class A(uuid: Int, rep: Seq[B])
case class Result(uuid: Int, mKey: Int, mValue: Int)

class NestedDataTest extends FlatSpec with Matchers{
  val spark: SparkSession = SparkUtils.spark
  import spark.implicits._



  "Flat data using FlatMap" should "explode" in {
    val df: Dataset[A] = Seq(
      A(
        123,
        Seq(
          B(
            Map(1 -> Seq(C(111), C(1111)), 2 -> Seq(C(222)))
          ),
          B(
            Map(3 -> Seq(C(333)), 4 -> Seq(C(444)))
          )
        )
      )
    ).toDS

    df
      .map( element => {
        val uuid: Int = element.uuid
        val seqB: Seq[(Int, Int)] = element.rep
          .flatMap { // will flatten Map
            mapOfB =>
              mapOfB.m.toSeq.flatMap { // will flatten the value of Map (Sequence of type C)
                case (key, seqOfValue) => seqOfValue.map(classC => (key, classC.value))
              }
          }
        seqB.map{ case (key, value) => Result(uuid, key, value)}
      })
      .withColumn("value", explode(col("value")))
      .show(false)
  }

}
