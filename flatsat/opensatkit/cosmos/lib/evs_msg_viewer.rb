# encoding: ascii-8bit

# Copyright 2014 Ball Aerospace & Technologies Corp.
# All Rights Reserved.
#
# This program is free software; you can modify and/or redistribute it
# under the terms of the GNU General Public License
# as published by the Free Software Foundation; version 3 with
# attribution addendums as found in the LICENSE.txt

require 'cosmos'

module Cosmos

  class EvsMsgViewer < Qt::Widget
    attr_reader :tab_name
    attr_reader :packets
    attr_reader :text

    # Create a component to go inside the DataViewer
    #
    # @param parent [Qt::Widget] Parent widget
    # @param tab_name [String] Name of the tab which displays this widget
    def initialize(parent, tab_name)
      super(parent)
      @tab_name = tab_name
      @packets = []
      @processed_queue = Queue.new
      @log_file_directory = System.paths['LOGS']
      @max_block_count = 1000
      @timer = nil
    end

    # Adds a packet to the list of packets this component processes
    #
    # @param target_name [String] Name of the target
    # @param packet_name [String] Name of the packet
    def add_packet(target_name, packet_name)
      @packets << [target_name, packet_name]
    end

    # Builds the gui for this component
    def initialize_gui
      @top_layout = Qt::VBoxLayout.new

      @text = Qt::PlainTextEdit.new
      @text.setReadOnly(true)
      @text.setMaximumBlockCount(@max_block_count)
      @text.font = Cosmos.get_default_small_font
      @text.setWordWrapMode(Qt::TextOption::NoWrap)
      @top_layout.addWidget(@text)

      @button = Qt::PushButton.new('Save Text to File')
      @button.connect(SIGNAL('clicked()')) do
        Qt.execute_in_main_thread(true) do
          filename = File.join(@log_file_directory, File.build_timestamped_filename(['dataviewer']))

          # Prompt user for filename
          filename = Qt::FileDialog::getSaveFileName(self, "Save As...", filename, "Text Files (*.txt);;All Files (*)")
          if not filename.nil? and not filename.empty?
            @log_file_directory = File.dirname(filename)
            @log_file_directory << '/' unless @log_file_directory[-1..-1] == '\\'

            File.open(filename, 'w') do |file|
              file.write(@text.toPlainText)
            end
          end
        end
      end
      @top_layout.addWidget(@button)

      @timer = Qt::Timer.new(self)
      @timer.connect(SIGNAL('timeout()')) { scroll_to_bottom() }
      @timer.setSingleShot(true)

      setLayout(@top_layout)
    end

    # Updates the gui with any changes if needed
    def update_gui
      begin
        loop do
          # Get new processed text
          processed_text = @processed_queue.pop(true)

          # Add text to text widget
          @text.appendPlainText(processed_text)
        end
      rescue ThreadError
        # Nothing to do
      end
    end

    # Resets the gui and any intermediate processing
    def reset
      @text.setPlainText("")
    end

    def showEvent(event)
      # When the tab is shown we want to ensure the scroll bar is at the
      # maximum to allow the PlainTextArea to automatically hold the scroll
      # at the bottom of the display while appending things.
      # If this is not done, switching tabs will cause the scroll bar
      # to "stick" and not stay at the bottom with the newest text.
      @timer.start(100)
    end

    def scroll_to_bottom
      @text.verticalScrollBar.value = @text.verticalScrollBar.maximum
    end

    # Processes the given packet. No gui interaction should be done in this
    # method. Override this method for other components.
    def process_packet(packet)
      items = packet.sorted_items
      str = ""
      str << "#{packet.read_item(items[1], :WITH_UNITS, packet.buffer)}"
      str << "  "
      str << "#{packet.read_item(items[8], :WITH_UNITS, packet.buffer)}"
      str << ": "
      str << "#{packet.read_item(items[13], :WITH_UNITS, packet.buffer)}"
      if @processed_queue.length < 1000
        @processed_queue << str
      end


#      processed_text = ''
#      processed_text << '*' * 80 << "\n"
#      processed_text << "* #{packet.target_name} #{packet.packet_name}\n"
#      processed_text << "* Received Time: #{packet.received_time.formatted}\n" if packet.received_time
#      processed_text << "* Received Count: #{packet.received_count}\n"
#      processed_text << '*' * 80 << "\n"
#      processed_text << packet.formatted(:WITH_UNITS) << "\n"
#      # Ensure that queue does not grow infinitely while paused
#      if @processed_queue.length < 1000
#        @processed_queue << processed_text
#      end

    end

    # Shutdown the Data Viewer Component. Called when program is closed.
    def shutdown
      # Do nothing by default
    end
  end

end # module Cosmos
