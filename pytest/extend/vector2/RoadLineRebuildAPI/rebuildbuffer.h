#ifndef __REBUILDBUFER_H__
#define __REBUILDBUFER_H__

namespace RoadRebuild
{
	/**
	* @brief 读取774的2.1矢量路口扩大图协议
	* @param type	1 表示在线协议，2表示离线协议
	* @param buf	二进制buffer
	* @param len	buffer长度
	* @return void *	MediaObject句柄
	**/
	void *_MemoryImport21(Gint32 type, void *buf, Guint32 len);

	Gbool _VectorExport(void *handle, Gpstr savefile);
	Gbool _VectorExportBuf(void *handle, void **pBuf, Guint32 &size);

};
#endif // __REBUILDBUFER_H__