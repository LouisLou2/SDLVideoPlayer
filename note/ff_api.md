### avformat_seek_file
函数会尝试将文件指针定位到目标时间戳 ts。
定位时会确保从所有**活动流（active streams）**可以成功解码和呈现内容。
所有的活动流指的是 AVStream 中 discard 属性小于 AVDISCARD_ALL 的流，这些流被认为是活动的，意味着它们不会被丢弃。
定位操作会受到 min_ts 和 max_ts 的限制：
min_ts 表示最小可接受的时间戳（通常为起始位置）。
max_ts 表示最大可接受的时间戳（通常为结束位置）。
定位的目标是找到一个在 min_ts 和 max_ts 范围内、最接近 ts 的位置，从而确保从该位置开始能够成功解码所有活动流。
标志（flags）的作用
AVSEEK_FLAG_BYTE：

如果 flags 中包含 AVSEEK_FLAG_BYTE，那么所有时间戳都以字节为单位，而不是通常的时间单位（如秒或微秒）。
在这种模式下，ts、min_ts 和 max_ts 都表示文件中的字节偏移量（byte offset），即直接定位到文件中的某个字节位置。
注意：并不是所有的解复用器（demuxers）都支持这种基于字节的 seek 方式。
AVSEEK_FLAG_FRAME：

如果 flags 中包含 AVSEEK_FLAG_FRAME，则所有时间戳都以帧为单位。
在这种模式下，ts、min_ts 和 max_ts 都表示帧的索引，而不是时间戳。
只有当 stream_index 指定了一个具体的流时，该流的帧索引才生效。并且这种模式并不是所有解复用器都支持。
默认时间戳单位（时间基）：

如果 flags 中不包含 AVSEEK_FLAG_BYTE 和 AVSEEK_FLAG_FRAME，则时间戳的单位由 stream_index 决定：
如果 stream_index 为 -1，时间戳将被视为 AV_TIME_BASE 单位，即默认时间基（单位通常为微秒）。
如果 stream_index 指定了某个流，则时间戳将基于该流的时间基（time base）。
AVSEEK_FLAG_ANY：

如果 flags 中包含 AVSEEK_FLAG_ANY，则非关键帧（non-keyframe）会被视为关键帧（keyframe）来处理。
这意味着，函数会尝试跳转到任何一个帧而不是严格跳转到关键帧。这种方式可能会导致解码器在该帧之前解码失败（因为跳转到非关键帧时，可能缺少解码该帧所需要的参考帧）。
AVSEEK_FLAG_BACKWARD：

如果 flags 中包含 AVSEEK_FLAG_BACKWARD，则表示要向后查找最接近目标时间戳的帧，而不是向前查找。
在描述中提到，AVSEEK_FLAG_BACKWARD 在某些情况下可能被忽略。

返回值大于等于 0 表示 seek 成功。

当 avformat_seek_file 函数中 flags 参数设置为 0 时，表示使用默认的 seek 行为。以下是 flags = 0 时的具体含义：

flags = 0 时的行为
当 flags 设置为 0 时，函数的 seek 行为如下：

时间戳单位为 stream_index 所选流的时间基准：

如果 stream_index 设置为 -1，则时间戳 ts、min_ts 和 max_ts 的单位为 AV_TIME_BASE（FFmpeg 的全局时间基准，单位为微秒）。
如果 stream_index 设置为某个具体的流（如视频流或音频流的索引），则时间戳的单位基于该流的时间基（time base）。时间基是一个流中每个时间单位（通常以秒为单位）包含的时间戳数量。
在关键帧上查找：

默认情况下（即 flags 为 0 时），avformat_seek_file 会尝试在目标时间戳 ts 附近的**关键帧（keyframe）**上进行查找。
如果目标时间戳 ts 落在两个关键帧之间，FFmpeg 通常会跳转到上一个关键帧的位置。
查找方向由 ts 与当前位置的关系决定：

默认行为不会强制向前或向后查找，而是根据目标时间戳 ts 与当前文件指针位置之间的关系来自动决定查找方向。
如果 ts 位于当前文件指针的后面，则向前查找；如果 ts 位于当前文件指针的前面，则向后查找。
不会视非关键帧为关键帧：

因为 flags 中没有包含 AVSEEK_FLAG_ANY，所以 avformat_seek_file 会严格在关键帧上查找，不会将非关键帧视为关键帧。这意味着如果目标时间戳 ts 附近只有非关键帧，FFmpeg 可能会跳转到上一个关键帧的位置，从而导致解码器能成功解码出完整的帧。
不进行字节级查找：

因为 flags 中没有 AVSEEK_FLAG_BYTE 标志，所以 avformat_seek_file 会使用时间戳（而非文件字节偏移）来进行查找。
这意味着 ts、min_ts 和 max_ts 会被解释为时间戳，而不是字节偏移量。
时间戳查找单位是帧而非字节或其他单位：

因为 flags 中没有 AVSEEK_FLAG_FRAME 标志，所以查找的单位是基于时间的，而不是基于帧索引的。