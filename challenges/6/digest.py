import ballcosmos

# Define a digest object for collection of telemetry data

class Digest:
    tlmentries = []

    def run(self):
        print(f"dig: Running Digest object with tlmentries={self.tlmentries}")

        # compiles a list of commands and runs them

        output = ""
        for t in self.tlmentries:
            output += ballcosmos.tlm(t)

        return output