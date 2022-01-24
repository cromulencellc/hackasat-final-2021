class CBuffer

  def initialize(max)
    # @f = @b = @fc = 0
    @head = 0
    @tail = 0
    @max = max
    @buffer = Array.new(@max)
    @full = false
  end

  def advance_pointer
    if (@full)
        @tail = (@tail + 1) % @max
    end
    @head = (@head + 1) % @max
    # We mark full because we will advance tail on the next time around
    @full = (@head == @tail)
  end

  def retreat_pointer
    @full = false
    @tail = (@tail + 1) % @max
  end

  def buf_reset
    @head = 0
    @tail = 0
    @full = false
  end

  def buf_size
    size = @max
    if (!@full)
        if (@head >= @tail)
            size = @head - @tail
        else
            size = @max + @head - @tail
        end
    end
    return size
  end

  def buf_capacity
    return @max
  end

  #overwrites old data if full
  def put(data)
    for i in (0..data.size()-1)
        @buffer[@head] = data[i]
        advance_pointer()
    end
  end

  #only adds data if not full returns error if full
  def put2(data)
    r = -1
    for i in (0..data.size()-1)
        if (!@full)
            @buffer[@head] = data[i]
            advance_pointer()
            r = 0
        end
    end
    return r
  end

  def get(number)
    data = []
    for i in (0..number-1)
        if (!buf_empty())
            data[i] = @buffer[@tail]
            retreat_pointer()
        end
    end
    return data
  end

  def buf_empty
    return (!@full && (@head == @tail))
  end

  def buf_full
    return @full
  end

  def to_s
    "<#{self.class} @tail=#{@tail} @head=#{@head} @full=#{@full}>"
  end
end