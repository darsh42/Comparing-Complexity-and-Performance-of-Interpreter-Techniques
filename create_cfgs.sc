@main def create_cfgs(output: String, functions: String) = {
  import java.nio.file.{Files, Paths}
  import io.shiftleft.semanticcpg.language._
  import io.joern.dataflowengineoss.language._

  val methods = (
    "interpreter_switch",
    "interpreter_computed_goto",
    "interpreter_decoupled",
    "interpreter_blocked",
    "interpreter_blocked_chaining")

  // define the output path
  val output_path = Paths.get(output)

  // create output path 
  if (!Files.exists(output_path)) {
    Files.createDirectories(output_path);
  }

  val _functions =
    functions.split(",").map(_.trim).filter(_.nonEmpty).toSeq

  // generate cfgs
  cpg.method.name(_functions: _*).foreach { method =>
      method.dotCfg.l #> s"${output_path.resolve(method.name).toString}.dot"
  }
}
