# encoding:utf-8

from xml.dom import minidom


def custom_route(request):
    dom = minidom.parseString(request)
    root = dom.documentElement
    data = root.getAttribute('ContentOptions')
    if data:
        content_options = (int(data, 16) | 0x02) if '0x' in data else (int(data, 10) | 0x02)
    else:
        content_options = 0x02
    root.setAttribute('ContentOptions', str(content_options))
    root.setAttribute('Vers', '4.0')
    return root.toxml('utf-8')


def get_path_shape(path):

    # 路径形状：道路ID相同视为路径形状相同

    link_lst = list()
    for i in range(0, path.m_seg_num):
        segment = path.m_seg_lst[i]
        for j in range(0, segment.m_link_num):
            link = segment.m_link_lst[j]
            if link.has_id_64():
                link_lst.append(link.get_topo_id_64())
    return link_lst


def get_path_link_dist_table(path):

    # 道路ID和道路距离的映射表

    table = dict()
    for i in range(0, path.m_seg_num):
        segment = path.m_seg_lst[i]
        for j in range(0, segment.m_link_num):
            link = segment.m_link_lst[j]
            if link.has_id_64():
                table[link.get_topo_id_64()] = link.get_dist()
    return table


def get_path_coincide(dst_path, ref_path):

    # 路径重合度：路径重合部分的长度和路径总长度的比值

    dst_shape = get_path_shape(dst_path)
    ref_shape = get_path_shape(ref_path)

    dist_table = get_path_link_dist_table(dst_path)

    tmp_dst_set = set(dst_shape)
    tmp_ref_set = set(ref_shape)

    coincident_set = tmp_dst_set & tmp_ref_set
    coincident_dist = 0
    for link64Id in coincident_set:
        coincident_dist += dist_table[link64Id]

    return coincident_dist/float(dst_path.m_Dist)


def make_etatrafficupdate_from_route():
    """Return etatrafficupdate request.
        Make etatrafficupdate request with route result.
    """
    pass
